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
int server_fd ; //sockets
struct sockaddr_in address ; // for tcp connection
int addrlen = sizeof(address);

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

    /* attaching socket to the port [8080] */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
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