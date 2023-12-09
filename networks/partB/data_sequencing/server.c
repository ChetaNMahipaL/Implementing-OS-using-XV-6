#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include<pthread.h>

typedef struct data{
    char str[101];
    int sequence;
    int data_bytes;
    int total_chunks;
}data_sent;

int sfd;
struct sockaddr_in serverAddress, client;
socklen_t addr_size;

void sender_function()
{
    char send_msg[4096];
    for(int i=0;i<4096;i++)
        send_msg[i]='\0';
    fgets(send_msg,4096,stdin);
    send_msg[strlen(send_msg)-1]='\0';
    char* pointer = send_msg;
    int seq_num = 1;
    // int bytes=0;
    int length=0;

    data_sent** chunk = (data_sent**)malloc(sizeof(data_sent*)*(strlen(send_msg)/100 + 1));
    for(int i=0;i<(strlen(send_msg)/100 + 1);i++)
    {
        chunk[i]=(data_sent*)malloc(sizeof(data_sent)* 1);
        if(strlen(send_msg) % 10 == 0)
        {
            chunk[i]->total_chunks=strlen(send_msg)/100;
        }
        else
        {
            chunk[i]->total_chunks=strlen(send_msg)/100 + 1;
        }
    }
    int* num_packets = (int*)malloc(sizeof(int));
    if(strlen(send_msg) % 100  == 0)
    {
        *num_packets = strlen(send_msg)/100;
    }
    else
    {
        *num_packets = strlen(send_msg)/100 + 1;
    }
    int send_sig=sendto(sfd,num_packets,sizeof(num_packets),0,(struct sockaddr*)&client, sizeof(serverAddress));
    if( send_sig == (-1))
    {
        printf("\033[1;31mERROR: Couldn't send message to server\n");
        printf("\033[0m");
        close(sfd);
        return;
    }   

    char final_str[101];
    for(int i=0;i<101;i++)
        final_str[i]='\0';

    int i=0;

    while(*pointer != '\0')
    {
        if(strlen(pointer) < 100)
        {
            length=strlen(pointer);
        }
        else 
        {
            length = 100;
        }
        strncpy(final_str,pointer,length);
        final_str[length]='\0';
        chunk[i]->sequence=seq_num;
        chunk[i]->data_bytes=length;
        strcpy(chunk[i]->str,final_str);
        chunk[i]->str[length]='\0';
        int send_sig=sendto(sfd,(data_sent*)&(*chunk[i]),sizeof(*chunk[i]),0,(struct sockaddr*)&client, sizeof(serverAddress));
        if( send_sig == (-1))
        {
            printf("\033[1;31mERROR: Couldn't send message to server\n");
            printf("\033[0m");
            continue;
        }
        seq_num = seq_num + length;
        i++;
        pointer = pointer + length;
    }
    for(int i=0;i<(strlen(send_msg)/100 + 1);i++)
        free(chunk[i]);
    free(chunk);
    // free(recv_chunk);
    free(num_packets);

    // return NULL;
}

void receiver_function()
{
    int* num_pack = (int *)malloc(sizeof(int));
    int recv_data=recvfrom(sfd, num_pack, sizeof(num_pack), 0, (struct sockaddr*)&client, &addr_size);
    if(recv_data < 0)
    {
        printf("\033[1;31mERROR: Nothing was recieved from client\n");
        printf("\033[0m");
        close(sfd);
        return;
    }
    // *num_pack = 2;
    data_sent** chunk = (data_sent**)malloc(sizeof(data_sent*)*(*num_pack));
    for(int i=0;i<(*num_pack);i++)
    {
        chunk[i]=(data_sent*)malloc(sizeof(data_sent)* 1);
    }    
    data_sent* recv_chunk = (data_sent*)malloc(sizeof(data_sent)*1);
    int count=(-1);
    int index;
    while(1)
    {
        int recv_data=recvfrom(sfd, recv_chunk, sizeof(*recv_chunk), 0, (struct sockaddr*)&client, &addr_size);
        if(recv_data < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from client\n");
            printf("\033[0m");
            continue;
        }
        index=recv_chunk->sequence/100;
        chunk[index]->data_bytes=recv_chunk->data_bytes;
        chunk[index]->sequence=recv_chunk->sequence;
        strncpy(chunk[index]->str,recv_chunk->str,recv_chunk->data_bytes);
        chunk[index]->str[recv_chunk->data_bytes]='\0';
        chunk[index]->total_chunks=recv_chunk->total_chunks;
        // index = index + 1;
        count = count + 1;
        if(count >= (*num_pack-1))
        {
            break;
        }
    }
    // close(sfd);
    char print_str[4096];
    print_str[0]='\0';
    for(int i=0;i<(*num_pack);i++)
    {
        if(chunk[i]->str[0] != '\0')
        {
            strcat(print_str,chunk[i]->str);
        }
    }
    printf("%s\n", print_str);
    for(int i=0;i<(*num_pack);i++)
        free(chunk[i]);
    free(chunk);
    free(recv_chunk);
    free(num_pack);

    // return NULL;
}

int main() {

    char* ip_address;    
    struct hostent *host_entry;
    char hostbuffer[256];
    char *IPbuffer;
    int hostname;

    int port = 7373;

    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if(hostname != 0)
    {
        printf("\033[1;31mERROR: Error in obtaining IP address\n");
        printf("\033[0m");
        return 0;        
    }
    host_entry = gethostbyname(hostbuffer);
    if(host_entry == NULL)
    {
        printf("\033[1;31mERROR: Error in obtaining IP address\n");
        printf("\033[0m");
        return 0;         
    }
    ip_address = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    // printf("%s\n", ip_address);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        printf("\033[1;31mERROR: Socket not created\n");
        printf("\033[0m");
        return 0;
    }

    memset(&serverAddress, '\0', sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int bind_fd = bind(sfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if ( bind_fd == (-1))
    {
        printf("\033[1;31mERROR: Couldn't bind socket\n");
        printf("\033[0m");
        return 0;
    }

    addr_size = sizeof(client);

    receiver_function();
    sender_function();

    close(sfd);

    return 0;
}
