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
/* ------------------ */

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
    char *message = "Hello Server. It's me(client)!";
    int valread;
    int n, len;

    Create_TCP_Connection();

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, message);
    write(client_fd, buffer, sizeof(buffer));


    //printf("Message from server: ");
    if ((valread = read(client_fd, buffer, 1024)) == 0)
    {
        /* Somebody disconnected , get his details and print */
        getpeername(client_fd, (struct sockaddr *)&servaddr, (socklen_t *)&servaddr);
        printf("Host disconnected , ip %s , port %d \n",
               inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

        /* Close the socket and mark as 0 in list for reuse */
        //close(client_fd);
    }

    /* Echo back the message that came in */
    else
    {
        /* set the string terminating NULL byte on the end
         * of the data read */
        buffer[valread] = '\0';
        printf("the receievd message is: %s\n", buffer);

        if (send(client_fd, message, strlen(message), 0) < 0)
        {
            perror("sending message failed");
        }
        else
            printf("the sent message : %s \n", message);
    }

    // read(client_fd, buffer, sizeof(buffer));
    // puts(buffer);

    return 0;
}