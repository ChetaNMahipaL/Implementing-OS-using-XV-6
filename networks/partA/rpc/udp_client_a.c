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

int main() {

    char* ip_address;    
    struct hostent *host_entry;
    char hostbuffer[256];
    char *IPbuffer;
    int hostname;

    int port = 8800;

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

    int sfd;
    socklen_t addr_size;
    struct sockaddr_in serverAddress;


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
    serverAddress.sin_addr.s_addr = inet_addr(ip_address);
    addr_size=sizeof(serverAddress);

    while(1)
    {
        char server_response[256];
        for(int y=0;y<256;y++)
        {
            server_response[y]='\0';
        }

        char input[256];
        for(int y=0;y<256;y++)
        {
            input[y]='\0';
        }
        input[0]='\0';
        printf("Enter your move: ");
        scanf("%s", &input);

        int send_sig=sendto(sfd,input,strlen(input),0,(struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if( send_sig == (-1))
        {
            printf("\033[1;31mERROR: Couldn't send message to server\n");
            printf("\033[0m");
            close(sfd);
            return 0;
        }

        int recv_data=recvfrom(sfd,server_response,1024,0,(struct sockaddr*)&serverAddress, &addr_size);
        if(recv_data < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from server\n");
            printf("\033[0m");
            close(sfd);
            return 0;
        }
        printf("%s \n",server_response);
        input[0]='\0';
        scanf("%s",&input);
        send_sig=sendto(sfd,input,strlen(input),0,(struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if( send_sig == (-1))
        {
            printf("\033[1;31mERROR: Couldn't send message to server\n");
            printf("\033[0m");
            close(sfd);
            return 0;
        }
        server_response[0]='\0';
        recv_data=recvfrom(sfd,server_response,1024,0,(struct sockaddr*)&serverAddress, &addr_size);
        if(recv_data < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from server\n");
            printf("\033[0m");
            close(sfd);
            return 0;
        }
        if(strcmp(input,"No")==0 || atoi(server_response) == 0)
        {
            // free(input);
            break;
        }
    }

    close(sfd);
    return 0;
}
