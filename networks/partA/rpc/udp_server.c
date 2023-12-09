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

    int port_A = 8800;  
    int port_B = 8801;    

    int sfd_A;
    int sfd_B;
    struct sockaddr_in serverAddress_A, serverAddress_B, client_A, client_B;
    socklen_t addr_size_A;
    socklen_t addr_size_B;

    sfd_A = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_A == -1)
    {
        printf("\033[1;31mERROR: Socket not created\n");
        printf("\033[0m");
        return 0;
    }
    sfd_B = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_B == -1)
    {
        printf("\033[1;31mERROR: Socket not created\n");
        printf("\033[0m");
        return 0;
    }

    memset(&serverAddress_A, '\0', sizeof(serverAddress_A));

    serverAddress_A.sin_family = AF_INET;
    serverAddress_A.sin_port = htons(port_A);
    serverAddress_A.sin_addr.s_addr = INADDR_ANY;

    memset(&serverAddress_B, '\0', sizeof(serverAddress_B));

    serverAddress_B.sin_family = AF_INET;
    serverAddress_B.sin_port = htons(port_B);
    serverAddress_B.sin_addr.s_addr = INADDR_ANY;

    addr_size_A = sizeof(client_A);
    addr_size_B = sizeof(client_B);
    int bind_fd_A = bind(sfd_A, (struct sockaddr*)&serverAddress_A, sizeof(serverAddress_A));
    if ( bind_fd_A == (-1))
    {
        printf("\033[1;31mERROR: Couldn't bind socket\n");
        printf("\033[0m");
        close(sfd_A);
        close(sfd_B);
        return 0;
    }
    int bind_fd_B = bind(sfd_B, (struct sockaddr*)&serverAddress_B, sizeof(serverAddress_B));
    if ( bind_fd_B == (-1))
    {
        printf("\033[1;31mERROR: Couldn't bind socket\n");
        printf("\033[0m");
        close(sfd_A);
        close(sfd_B);
        return 0;
    }

    while(1)
    {
        char client_response_A[256];
        for(int y=0;y<256;y++)
        {
            client_response_A[y]='\0';
        }
        int recv_data_A=recvfrom(sfd_A, client_response_A, 1024, 0, (struct sockaddr*)&client_A, &addr_size_A);
        if(recv_data_A < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);
            return 0;
        }
        char client_response_B[256];
        for(int y=0;y<256;y++)
        {
            client_response_B[y]='\0';
        } 
        int recv_data_B=recvfrom(sfd_B, client_response_B, 1024, 0, (struct sockaddr*)&client_B, &addr_size_B);
        if(recv_data_B < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);
            return 0;
        }

        char* send_msg_A= (char*)calloc(256,sizeof(char));
        char* send_msg_B= (char*)calloc(256,sizeof(char));


        if(strcmp(client_response_A,client_response_B) == 0 )       // 0-rock, 1-paper, 2-scissors
        {
            strcpy(send_msg_A,"The result of match is: \"Draw\"\nDo you want to play again?");
            strcpy(send_msg_B,"The result of match is: \"Draw\"\nDo you want to play again?");
        }
        else if((atoi(client_response_A) == 0 && atoi(client_response_B)== 2) ||
                (atoi(client_response_A) == 1 && atoi(client_response_B)== 0) ||
                (atoi(client_response_A) == 2 && atoi(client_response_B)== 1))
                {
                    strcpy(send_msg_A,"You have won the match\nDo you want to play again?");
                    strcpy(send_msg_B,"You have lost the match\nDo you want to play again?");
                }
        else
        {
            strcpy(send_msg_B,"You have won the match\nDo you want to play again?");
            strcpy(send_msg_A,"You have lost the match\nDo you want to play again?");            
        }

        int send_sig_A=sendto(sfd_A,send_msg_A,strlen(send_msg_A),0,(struct sockaddr*)&client_A, sizeof(client_A));
        if( send_sig_A == (-1))
        {
            printf("\033[1;31mERROR: Couldn't send message to client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);
            return 0;
        }
        int send_sig_B=sendto(sfd_B,send_msg_B,strlen(send_msg_B),0,(struct sockaddr*)&client_B, sizeof(client_B));
        if( send_sig_B == (-1))
        {
            printf("\033[1;31mERROR: Couldn't send message to client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);
            return 0;
        }

        client_response_A[0]='\0';
        client_response_B[0]='\0';

        recv_data_A=recvfrom(sfd_A, client_response_A, 1024, 0, (struct sockaddr*)&client_A, &addr_size_A);
        if(recv_data_A < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);            
            return 0;
        }
        recv_data_B=recvfrom(sfd_B, client_response_B, 1024, 0, (struct sockaddr*)&client_B, &addr_size_B);
        if(recv_data_B < 0)
        {
            printf("\033[1;31mERROR: Nothing was recieved from client\n");
            printf("\033[0m");
            close(sfd_A);
            close(sfd_B);
            return 0;
        }

        client_response_A[3]='\0';
        client_response_B[3]='\0';

        if(strcmp(client_response_A,"Yes") == 0 && strcmp(client_response_B,"Yes") == 0)
        {
            free(send_msg_A);
            free(send_msg_B);
            // close(con_A);
            // close(con_B);
            send_sig_A=sendto(sfd_A,"1",1,0,(struct sockaddr*)&client_A, sizeof(client_A));
            if( send_sig_A == (-1))
            {
                printf("\033[1;31mERROR: Couldn't send message to client A\n");
                printf("\033[0m");
                close(sfd_A);
                close(sfd_B);
                return 0;
            }
            send_sig_B=sendto(sfd_B,"1",1,0,(struct sockaddr*)&client_B, sizeof(client_B));
            if( send_sig_B == (-1))
            {
                printf("\033[1;31mERROR: Couldn't send message to client B\n");
                printf("\033[0m");
                close(sfd_A);
                close(sfd_B);
                return 0;
            }
            continue;
        }
        else
        {
            free(send_msg_A);
            free(send_msg_B);
            send_sig_A=sendto(sfd_A,"0",1,0,(struct sockaddr*)&client_A, sizeof(client_A));
            if( send_sig_A == (-1))
            {
                printf("\033[1;31mERROR: Couldn't send message to client A\n");
                printf("\033[0m");
                close(sfd_A);
                close(sfd_B);
                return 0;
            }
            send_sig_B=sendto(sfd_B,"0",1,0,(struct sockaddr*)&client_B, sizeof(client_B));
            if( send_sig_B == (-1))
            {
                printf("\033[1;31mERROR: Couldn't send message to client B\n");
                printf("\033[0m");
                close(sfd_A);
                close(sfd_B);
                return 0;
            }
            // close(con_A);
            // close(con_B);
            break;            
        }

    }

    close(sfd_A);
    close(sfd_B);

    return 0;
}
