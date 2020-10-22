#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define MAXSIZE 1024 /*max buffer size*/
#define SERV_PORT 8080 /*port*/
#define LISTENQ 3 /*maximum number of client connections*/
#define MAX_CLIENTS_COUNT 20

/* declaring variables*/
int server_fd, new_socket ;   //sockets
struct sockaddr_in address ; // for tcp connection
int addrlen = sizeof(address) ;

fd_set client_fds ; /* set of file descriptors of clients */
int max_fds ;
int clientSockets[MAX_CLIENTS_COUNT] = {0} ;
/* ------------------ */





void Create_TCP_Server_Port()
{
    int opt = 1;
    /* creating socket with IPv4 protocol and TCP type */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /*  set master socket to allow multiple connections */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;       // for IPv4 protocol
    address.sin_addr.s_addr = INADDR_ANY; // to bind the server to the localhost "127.0.0.1"
    address.sin_port = htons(SERV_PORT);  // host format to network format convertor

    /* binding socket to the port [8080] in localhost*/
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) <= 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    /* listening */
    if (listen(server_fd, LISTENQ) <= 0)
    {
        perror("listening failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    int event, valread;
    int sd;
    Create_TCP_Server_Port();

    char buffer[MAXSIZE+1]; //data buffer of 1K

    /* welcome message */
    char *message = "Hi you connected to server successfully \r\n";

    while(TRUE)
    {
        /* clear the descriptor set */
        FD_ZERO(&client_fds);

        /* set server_fd in readset */
        FD_SET(server_fd, &client_fds);

        /* set max_fds */
        max_fds = server_fd;

        /* add child sockets to set */
        for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        {
            //socket descriptor
            sd = clientSockets[i];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &client_fds);

            //highest file descriptor number, need it for the select function
            if (sd > max_fds)
                max_fds = sd;
        }

        /* wait for an activity on one of the sockets , timeout is NULL ,
         * so wait indefinitely */
        event = select(max_fds + 1, &client_fds, NULL, NULL, NULL);

        if ((event < 0) && (errno != EINTR))
        {
            perror("select() failed");
        }

        /* If something happened on the master socket ,
         * then its an incoming connection */
        if (FD_ISSET(server_fd, &client_fds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accepting faild");
                exit(EXIT_FAILURE);
            }

            /* inform user of socket number - used in send and receive commands */
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket ,
                inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            /* send new connection greeting message */
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
            {
                perror("Welcome message not sent !");
            }

            puts("Welcome message sent successfully");

            /* add new socket to array of sockets */
            for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
            {
                //if position is empty
                if (clientSockets[i] == 0)
                {
                    clientSockets[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        /* else its some IO operation on some other socket */
        for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        {
            sd = clientSockets[i];

            if (FD_ISSET(sd, &client_fds))
            {
                /* Check if it was for closing , and also read the
                 * incoming message */
                if ((valread = read(sd, buffer, 1024)) == 0)
                {
                    /* Somebody disconnected , get his details and print */
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                        inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    /* Close the socket and mark as 0 in list for reuse */
                    close(sd);
                    clientSockets[i] = 0;
                }

                /* Echo back the message that came in */
                else
                {
                    /* set the string terminating NULL byte on the end
                     * of the data read */
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    return 0;
}