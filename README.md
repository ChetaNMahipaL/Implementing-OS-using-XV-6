# Intro to Xv6

# XV6 Custom System Call (`readcount`) and Signal Handling (`sigalarm`)

## Custom System Call: `readcount`

### Overview
This XV6 project extends the operating system by adding a custom system call called `readcount`. The purpose of this system call is to retrieve the count of successful reads performed by a specific process.

### Implementation Steps

1. **Define the System Call Number**
   - Choose an unused system call number in `syscall.h` (e.g., add `#define SYS_readcount 22`).

2. **Declare the System Call Prototype**
   - Add the declaration of the `readcount` system call in `syscall.c` and `sysproc.c`.

3. **Implement the System Call**
   - Implement the actual functionality of the `readcount` system call in `sysfile.c` or a suitable location. This involves keeping track of read counts for each process.

4. **User-Space Wrapper**
   - Create a user-space wrapper function for the `readcount` system call in the `usys.S` or a similar file.

5. **User Program Usage**
   - Update user programs to use the new `readcount` system call by including the appropriate header file and calling the user-space wrapper function.

## Signal Handling: `sigalarm`

### Overview
In addition to the `readcount` system call, this XV6 project introduces signal handling for the `sigalarm` signal. The goal is to implement an alarm mechanism that allows a process to set an alarm and receive a signal when the specified time has elapsed.

### Implementation Steps

1. **Define the Signal Number**
   - Choose an unused signal number for `sigalarm` in `signal.h` (e.g., add `#define SIGALRM 14`).

2. **Handle the Signal in `trap.c`**
   - Update the `trap.c` file to handle the `sigalarm` signal. This involves adding a case in the `trap` function to recognize and respond to the `SIGALRM` signal.

3. **Signal Handler Function**
   - Implement a signal handler function for `sigalarm` in a suitable file (e.g., `signal.c`). This function will be called when a process receives the `sigalarm` signal.

4. **Set Alarm System Call**
   - Create a system call, such as `sys_alarm`, to allow processes to set an alarm. Implement this system call in `sysproc.c` and the corresponding kernel file.

5. **User Program Usage**
   - Update user programs to use the new alarm system call (`sys_alarm`) to set alarms and handle the `sigalarm` signal. This may involve including the appropriate header files and using the system call wrapper in user programs.

# XV6 Scheduling Algorithms

This repository contains implementations of two scheduling algorithms, First-Come-First-Serve (FCFS) and Multi-Level Feedback Queue (MLFQ), in the XV6 operating system.

## FCFS (First-Come-First-Serve)

### Overview

FCFS is a simple scheduling algorithm that executes processes in the order they arrive in the ready state (waiting in queue to get executed).

### Implementation

In XV6, FCFS is implemented in following steps:

Step 1: We introduce a variable in struct of processes that store the time it enters (or time of creation).

Step 2: In void scheduler() in kernel/proc.c, we select the the process which has least time of creation
    
        struct proc* min_time_proc = proc;
        // Avoid deadlock by ensuring that devices can interrupt.
        intr_on();
        long int min = __INT64_MAX__;
        for (p = proc; p < &proc[NPROC]; p++)
        {
        if ((p->state == RUNNABLE) && (p->ctime < min))
        { 
            min_time_proc = p;
            min = p->ctime;
        }
        }
        
        p=min_time_proc;
        acquire(&p->lock);
        if (p->state == RUNNABLE)
        {
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
        }
        release(&p->lock);
        
Step 3: After this we have to make it non-premptive by disabling I/O in schedulertest.c and kernal and user trap in kernel/trap.c.

Step 4: After that you can run schdeduler test by compiling using "FCFS" flag.
## MLFQ (Multi-Level-FeedBack-Queue)

### Overview

Multi-Level Feedback Queue (MLFQ) is a scheduling algorithm used in operating systems that assigns different priorities to processes and dynamically adjusts these priorities based on their behavior and usage patterns.

### Implementation
Step 1:We introduce a variables in struct of processes that store information about the process and queues of MLFQ

    int level; // level or queue number
    int check_in; // check whether proc is in any queue or not
    int timeslice; // timeslice in the current queue
    int entry_time; // Entry time in this queue
    int run_time[4]; // run time in 4 queues
Step 2: We declare the Queue and it's aiding functions

    typedef struct node
    {
      struct proc* curr_proc;
      struct node* next;  
    }Node;
    
    typedef struct QUEUE
    {
      struct node* head;
      int curr_size;
    }queue;
    
    // aiding functions
    void enqueue(Node ** head, struct proc* p)
    {
      Node* temp = 0;
    
      for(int i=0;i<NPROC;i++)
      {
        if(lis_proc[i].curr_proc == 0)
        {
          temp = &(lis_proc[i]);
          break;
        }
      }
    
      temp->curr_proc=p;
      temp->next=0;
    
      if( *head == 0)
      {
        *head = temp; 
      }
      // Insertion at the end
      else
      {
        Node * start = *head;
        while (start->next != 0)
        {
          start=start->next;
        }
        start->next = temp;    
      }
    }
    
    struct proc* front(Node ** head)
    {
      if(*head == 0)
      {
        return 0;
      }
      Node* temp = *head;
      return(temp->curr_proc);
    };
    
    
    void dequeue(Node ** head)
    {
      if(*head == 0)
      {
        return;
      }
      Node* temp = *head;
      *head = (*head)->next;
      temp->curr_proc=0;
      temp->next=0; 
    }
    
    void delete(Node** head, uint pid)
    {
      if(*head == 0)
      {
        return;
      }  
      if((*head)->curr_proc->pid == pid)
      {
        Node* temp = *head;
        *head = (*head)->next;
        temp->curr_proc=0;
        temp->next=0;
        return;
      }
    
      Node * start = *head;
      Node * start_next = start->next;
      while(start_next !=0 && start != 0 && start_next->curr_proc->pid != pid )
      {
        start=start->next;
        start_next=start_next->next;
      }
      Node * temp = start_next;
      start->next = start_next->next;
      temp->curr_proc=0; 
      temp->next=0;
    }
    
Step 3: We initailize the Queues in void procinit() in kernel/proc.c

    for (int i = 0; i < 4; i++)
    {
        Queue[i].head=0;
        Queue[i].curr_size=0;
    }
    for(int i=0;i<NPROC;i++)
    {
        lis_proc[i].curr_proc=0;
        lis_proc[i].next=0;
    }
Step 4: We now make change to scheduler to schedule process, in which we first check the AGING (which is if process is being starved we move it to higher queue, aging ticks set to 32) and then takes process from queue level and run it

    //AGING OF THE PROCESS
      if (p != 0 && p->state == RUNNABLE && (ticks-(p->entry_time)) >= 32)
      {
        if(p->check_in == 1)
        {
          delete(&(Queue[p->level].head),p->pid);
          Queue[p->level].curr_size--;
          p->check_in=0;
        }
        if(p->level != 0)
        {
          p->level--;
        }
        p->entry_time = ticks;

      }
      for( p = proc; p < &proc[NPROC]; p++)
      {

        if(p!=0 && p->state == RUNNABLE && p->check_in == 0)
        {
          enqueue(&(Queue[p->level].head), p);
          Queue[p->level].curr_size++;
          p->check_in=1;
        }

      }
      struct proc* proc_to_end = 0;
      for(int q_lev = 0; q_lev < 4; q_lev++)
      {
        while(Queue[q_lev].curr_size)
        {
          p = front(&(Queue[q_lev].head));
          dequeue(&(Queue[q_lev].head));
          Queue[q_lev].curr_size--;
          p->check_in=0;
          if(p->state == RUNNABLE)
          {
            p->entry_time = ticks;
            proc_to_end = p;
            break;
          }
        }
        if(proc_to_end != 0)
        {
          break;
        }
      }
      if (proc_to_end == 0)
      {
        continue;
      }
      p=proc_to_end;
      acquire(&p->lock);
      if (p->state == RUNNABLE)
      {
        // p->timeslice = 1 << p->level;
        if(p->level == 0)
        {
          p->timeslice = 1;
        }
        else if(p->level == 1)
        {
          p->timeslice = 3;
        }
        else if(p->level == 2)
        {
          p->timeslice = 9;
        }
        else if(p->level == 3)
        {
          p->timeslice = 15;
        }
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        // printf("%d\n", p->timeslice);
        p->state = RUNNING;
        c->proc = p;
        p->entry_time = ticks;
        swtch(&c->context, &p->context);

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;      
      }
      release(&p->lock);
      
Step 5: Now we update the time slice in void update_time() in kernel/proc.c

    #ifdef MLFQ
        p->run_time[p->level]++;
        p->timeslice--;
    #endif
    
Step 6: At last we change the trap.c to change the priority (moving proc in between queues) based on timeslice used by process in kernaltrap and usertrap in kernel/trap.c

    struct proc* p = myproc();
    if( p!=0 && p->state == RUNNING)
    {
      if(p->timeslice <= 0)
      {
        if (p->level < 3)
        {
          p->level = p->level + 1;
          yield();
        }
      }
    }

Step 7: After that you can run schdeduler test by compiling using "MLFQ" flag.

### Comparision between scheduler

| Scheduling Algorithm  | Run Time | Wait Time |
|-----------------------|----------|-----------|
| RR                    | 9        | 136       |
| RR (CPU-2)            | 8        | 117       |
| FCFS                  | 18       | 84        |
| FCFS (CPU-2)          | 21       | 42        |
| MLFQ                  | 9        | 133       |

# UDP TO TCP
# Comparision Between Standard TCP and implemented TCP
### Similarity
1. The implemented TCP follows the same protocol of use of sequence number and acknowledgmenet number. We start with random sequence number (in this case ```seq_num = 1``` ). we increment the sequence number ( i.e. sequence number for next packet) to be sum of current sequence number and bytes of data sent in packet. In the same way, ```ACK``` is sum of length of data recieved and sequence number.
2. We check for ACK for a fixed period (i.e. 0.1 seconds in this case), if not received within time period we again send the package. This process keeps on going unitl we receive ACK for all the processes.
### Differences
1. A SYN bit (sequence number for first packet) is not sent to check for connection, instead we send the number of packets to be sent and make use of it to check connection.
2. We do not have window (which takes a maximum limit of packets in case ACK not received), we keep on sending the package whether or not ACK is received for it or not and then resend according to ACK check.
3. The packet size is fixed in our program, which means that if the string has 1024 bytes then except the last one each packet will have maximum size (i.e. if max size is 100 then there will be 10 packets of size 100 and last packet of size 24), unlike standard TCP in which size can vary.
4. The communication is one sided i.e. only one will receive at particular instant and other will send it (both sending and receive is not concurrent for one server).
## Extending to Account Flow Control
#### What is Flow Control?
Flow control in TCP regulates data transmission to match the receiver's processing capacity, preventing overload and ensuring efficient communication. It uses sliding windows and acknowledgments to manage data flow dynamically.
#### Sliding Window
TCP uses a sliding window mechanism to implement flow control. The sender and receiver each maintain a window of acceptable sequence numbers. The sender can only transmit data within the sender's window, and the receiver can only accept data within the receiver's window.
Follow the below steps to extend the implementation:

    Step 1: Create  a counter count.
    Step 2: We increment this counter and will send a limited number of packets (let's say 3 packets), if counter%3 == 0 we goto ACK check function.
    Step 3: We stay on ACK check until we receive that all the three packets received on other side.
    Step 4: We set counter back to 0 and start sending other packets.
    Step 5: Before sending we check the send_check bit (becomes 1 on successful ACK transmission) and send the first non checked packets.
    Step 6: We keep on doing this until all the packets have been sent.

    
    

