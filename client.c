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
}
