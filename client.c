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

#define MAXSIZE 1024   /*max buffer size*/
#define SERV_PORT 8080 /*port*/

/* declaring variables*/
int client_fd  ;   //sockets
struct sockaddr_in servaddr ; // for tcp connection

fd_set server_fds;
int max_fd;

int port_adr_recieved;
int client_turn;
int group_members;
/* ------------------ */

char **split(char *str)
{
    char **res = NULL;
    char *p = strtok(str, " ");
    int n_spaces = 0, i;

    /* split string and append tokens to 'res' */

    while (p)
    {
        res = realloc(res, sizeof(char *) * ++n_spaces);

        if (res == NULL)
            exit(-1); /* memory allocation failed */

        res[n_spaces - 1] = p;

        p = strtok(NULL, " ");
    }

    /* realloc one extra element for the last NULL */

    res = realloc(res, sizeof(char *) * (n_spaces + 1));
    res[n_spaces] = 0;

    /* free the memory allocated */
    return res;
    free(res);
}

void Create_TCP_Connection()
{
    /* creating socket with IPv4 protocol and TCP type */
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* reset the server address struct */
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;        // for IPv4 protocol
    servaddr.sin_addr.s_addr = INADDR_ANY; // to bind the server to the localhost "127.0.0.1"
    servaddr.sin_port = htons(SERV_PORT);  // host format to network format convertor

    if (connect(client_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Error : Connect Failed !");
    }
    else
        perror("connected to the server...");
}

int main(int argc, char const *argv[])
{
    char buffer[MAXSIZE+1];
    //char *message = "Hello Server. It's me(client)!";
    int event, valread;
    char **res = NULL;
    //int n, len;

    Create_TCP_Connection();

    
    // strcpy(buffer, message);
    // write(client_fd, buffer, sizeof(buffer));


    while (TRUE)
    {
        memset(buffer, 0, sizeof(buffer));
        /* clear the descriptor set */
        FD_ZERO(&server_fds);

        /* set client_fd in readset */
        FD_SET(client_fd, &server_fds);

        /* set stdin in readset */
        FD_SET(fileno(stdin), &server_fds);

        /* set max_fds */
        max_fd = client_fd;

        /* wait for an activity on one of the sockets , timeout is NULL ,
         * so wait indefinitely */
        event = select(max_fd + 1, &server_fds, NULL, NULL, NULL);

        if ((event < 0) && (errno != EINTR))
        {
            perror("select() failed");
        }

        /* If something happened on the server socket ,
         * then its an incoming connection */
        if (FD_ISSET(client_fd, &server_fds))
        {
            if ((valread = read(client_fd, buffer, 1024)) == 0)
            {
                /* Somebody disconnected , get his details and print */
                getpeername(client_fd, (struct sockaddr *)&servaddr, (socklen_t *)&servaddr);
                printf("Host disconnected , ip %s , port %d \n",
                       inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

                /* Close the socket and mark as 0 in list for reuse */
                close(client_fd);
            }

            /* Echo back the message that came in */
            else
            {
                /* set the string terminating NULL byte on the end
                * of the data read */
                buffer[valread] = '\0';
                printf("the receievd message is: %s\n", buffer);
                if (strlen(buffer) <= 9)
                {
                    res = split(buffer);
                    port_adr_recieved = atoi(res[0]);
                    client_turn = atoi(res[1]);
                    printf("received port address is: %d and game turn is : %d \n", port_adr_recieved, client_turn);
                    close(client_fd);
                    break;
                }
            }
        }

        /* else its some IO operation on some other socket */
        if (FD_ISSET(fileno(stdin), &server_fds))
        {
            printf("Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n");
            fgets(buffer, 1024, stdin);
            strtok(buffer, "\n");
            printf("You typed: %s\n", buffer);
            if (send(client_fd, buffer, strlen(buffer), 0) < 0)
            {
                perror("sending message failed");
            }
            else
                printf("the sent message : \"%s\" \n", buffer);
        }
    }

    // read(client_fd, buffer, sizeof(buffer));
    // puts(buffer);

    return 0;
}