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
#define LISTENQ 8 /*maximum number of client connections*/
#define MAX_CLIENTS_COUNT 20

