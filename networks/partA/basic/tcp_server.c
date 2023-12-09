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

    int sfd, con, length;
    struct sockaddr_in serverAddress, client;

    // creating a socket and verifying it.
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("\033[1;31mERROR: Socket not created\n");
        printf("\033[0m");
        return 0;
    }

    memset(&serverAddress, '\0', sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    int bind_fd = bind(sfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if ( bind_fd == (-1))
    {
        printf("\033[1;31mERROR: Couldn't bind socket\n");
        printf("\033[0m");
        close(sfd);
        return 0;
    }


    if ((listen(sfd, 5)) == (-1))
    {
        printf("\033[1;31mERROR: Nothing recieved, listening failed\n");
        printf("\033[0m");
        close(sfd);
        return 0;
    }
    
    length = sizeof(client);

    con = accept(sfd, (struct sockaddr *)&client, &length);
    if (con < 0)
    {
        printf("\033[1;31mERROR: Couldn't establish connection\n");
        printf("\033[0m");
        close(sfd);
        return 0;
    }

    char client_response[1024];

    int recv_data=recv(con,client_response,1024,0);
    if(recv_data < 0)
    {
        printf("\033[1;31mERROR: Nothing was recieved from client\n");
        printf("\033[0m");
        close(con);
        close(sfd);
        return 0;
    }
    printf("Recieved from client: %s\n",client_response);

    char* send_msg = "Hello! Recieved your message";

    int send_sig=send(con,send_msg,strlen(send_msg),0);
    if( send_sig == (-1))
    {
        printf("\033[1;31mERROR: Couldn't send message to client\n");
        printf("\033[0m");
        close(con);
        close(sfd);
        return 0;
    }
    close(con);
    close(sfd);
    return 0;
}