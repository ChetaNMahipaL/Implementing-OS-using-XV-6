#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_waitx(void)
{
  uint64 addr, addr1, addr2;
  uint wtime, rtime;
  argaddr(0, &addr);
  argaddr(1, &addr1); // user virtual memory
  argaddr(2, &addr2);
  int ret = waitx(addr, &wtime, &rtime);
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr1, (char *)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2, (char *)&rtime, sizeof(int)) < 0)
    return -1;
  return ret;
}

//  My code

uint64
sys_getreadcount(void)
{
  return count_read_call;
}

uint64
sys_sigalarm(void)    // https://courses.cs.washington.edu/courses/csep551/19au/labs/alarm.html
{
  int interval;
  uint64 handler;

  // implementation slightly different as argint is void type function but in link it is int type
  argint(0,&interval); // https://www.cse.iitd.ernet.in/~sbansal/os/previous_years/2011/xv6_html/syscall_8c.html
  argaddr(1,&handler); // https://stackoverflow.com/questions/76159582/how-to-pass-argument-from-system-call-to-user-space-in-xv6
  
  struct proc* p = myproc();

  p->sig_status=0;
  p->curr_ticks=0; // signal yet to be handled, called for first time
  p->interval=interval;
  p->sig_handler=handler;
  return 1;
}

uint64
sys_sigreturn(void)
{
  struct proc* p = myproc();

  memmove(p->trapframe,p->alarm_tf,PGSIZE); // restoring the context (saved registers)
  kfree(p->alarm_tf);
  p->curr_ticks=0;
  p->alarm_tf=0;
  p->sig_status=0; // disbale the sigalarm call
  return myproc()->trapframe->a0; //a0 used to store return value of function (signal handler)
}