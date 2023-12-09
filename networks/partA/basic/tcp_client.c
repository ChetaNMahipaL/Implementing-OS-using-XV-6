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


int main()
{
    char* ip_address;    
    struct hostent *host_entry;
    char hostbuffer[256];
    char *IPbuffer;
    int hostname;

    int port = 8080;

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
    struct sockaddr_in serverAddress;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("\033[1;31mERROR: Socket not created\n");
        printf("\033[0m");
        // close(sfd);
        return 0;
    }

    memset(&serverAddress, '\0', sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ip_address);
    serverAddress.sin_port = htons(port);

    
    int con = connect(sfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if(con == (-1))
    {
        printf("\033[1;31mERROR: Couldn't connect with server\n");
        printf("\033[0m");
        close(sfd);
        return 0;        
    }

    char server_response[1024];

    char* send_msg = "Hello! This side Client";

    int send_sig=send(sfd,send_msg,strlen(send_msg),0);
    if( send_sig == (-1))
    {
        printf("\033[1;31mERROR: Couldn't send message to server\n");
        printf("\033[0m");
        close(con);
        close(sfd);
        return 0;
    }

    int recv_data=recv(sfd,server_response,1024,0);
    if(recv_data < 0)
    {
        printf("\033[1;31mERROR: Nothing was recieved from server\n");
        printf("\033[0m");
        close(con);
        close(sfd);
        return 0;
    }
    printf("Recieved from server: %s\n",server_response);

    close(con);
    close(sfd);
    return 0;
}