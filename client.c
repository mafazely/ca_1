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
#define BROADCAST_IP_ADDR "127.255.255.250"

#define BOARD_WIDTH_FOUR 4
#define BOARD_WIDTH_THREE 3
#define BOARD_WIDTH_TWO 2

int mapH2[BOARD_WIDTH_TWO][BOARD_WIDTH_TWO + 1];  /* map for */
int mapV2[BOARD_WIDTH_TWO + 1][BOARD_WIDTH_TWO]; /*   two   */
int scores2[BOARD_WIDTH_TWO][BOARD_WIDTH_TWO];   /* players */

int mapH3[BOARD_WIDTH_THREE][BOARD_WIDTH_THREE + 1]; /* map for */
int mapV3[BOARD_WIDTH_THREE + 1][BOARD_WIDTH_THREE]; /*  three  */
int scores3[BOARD_WIDTH_THREE][BOARD_WIDTH_THREE];   /* players */

int mapH4[BOARD_WIDTH_FOUR][BOARD_WIDTH_FOUR + 1]; /* map for */
int mapV4[BOARD_WIDTH_FOUR + 1][BOARD_WIDTH_FOUR]; /*   four  */
int scores4[BOARD_WIDTH_FOUR][BOARD_WIDTH_FOUR];   /* players */

int write_desc_flag = 1; //flag for just one time print the description
/* declaring variables*/
int client_fd  ;   // TCP socket
struct sockaddr_in servaddr ; // for tcp connection
struct sockaddr_in broadcast_addr; // for UDP BroadCasting
struct sockaddr_in cli_addr;

fd_set server_fds;
int max_fd;

int port_adr_recieved;
int client_turn;
int group_members;

int udp_fd; // UDP socket
fd_set games_fds;
int max_udp_fd;
/* ------------------ */

char** split(char *str)
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
        //perror("socket creation failed");
        write(2, "socket failed\n", 14);
        exit(EXIT_FAILURE);
    }

    /* reset the server address struct */
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;        // for IPv4 protocol
    servaddr.sin_addr.s_addr = INADDR_ANY; // to bind the server to the localhost "127.0.0.1"
    servaddr.sin_port = htons(SERV_PORT);  // host format to network format convertor

    if (connect(client_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        //perror("Error : Connect Failed !");
        write(2, "connect to server failed\n", 25);
    }
    else
    {
        //perror("connected to the server...");
        write(1, "connected to the server...\n", 27);
    }
}

void Create_UDP_BroadCasting()
{
    int broadcast = 1;
    /* creating socket file descriptor */
    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        //perror("UDP socket creation failed");
        write(2, "UDP socket creation failed\n", 27);
        exit(EXIT_FAILURE);
    }

    /* attaching socket to the port [8080] and set broadcasting flag */
    if (setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_BROADCAST, &broadcast, sizeof(broadcast)))
    {
        //perror("UDP setsockopt failed");
        write(2, "UDP setsockopt failed\n", 22);
        exit(EXIT_FAILURE);
    }

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));

    /* Defining socket configures */
    broadcast_addr.sin_family = AF_INET;                        // for IPv4 protocol
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP_ADDR); // to bind the server to the broadcast ip
    broadcast_addr.sin_port = htons(port_adr_recieved);         // set recieved port from server

    /* Bind the socket with the server address  */
    if (bind(udp_fd, (const struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0)
    {
        //perror("UDP binding failed");
        write(2, "UDP binding failed\n", 19);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    char buffer[MAXSIZE+1];
    //char *message = "Hello Server. It's me(client)!";
    int event, valread;
    char **res = NULL;
    //int n, len;
    char str[MAXSIZE + 1];

    Create_TCP_Connection();

    /* ---- handling tcp connection and get port number and turn number ----*/
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
            //perror("select() failed");
            write(2, "select() failed\n", 16);
        }

        /* If something happened on the server socket ,
         * then its an incoming connection */
        if (FD_ISSET(client_fd, &server_fds))
        {
            if ((valread = read(client_fd, buffer, 1024)) == 0)
            {
                /* Somebody disconnected , get his details and print */
                getpeername(client_fd, (struct sockaddr *)&servaddr, (socklen_t *)&servaddr);
                //printf("Host disconnected , ip %s , port %d \n",
                //       inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
                write(1, "Host disconnected , ip : ", 25);
                write(1, inet_ntoa(servaddr.sin_addr), strlen(inet_ntoa(servaddr.sin_addr)));
                write(1, " , port ", 8);
                sprintf(str, "%d", ntohs(servaddr.sin_port));
                write(1, str, strlen(str));
                memset(str, 0, sizeof(str));
                write(1, " \n", 2);

                /* Close the socket and mark as 0 in list for reuse */
                close(client_fd);
            }

            /* Echo back the message that came in */
            else
            {
                /* set the string terminating NULL byte on the end
                * of the data read */
                buffer[valread] = '\0';
                //printf("the receievd message is: %s\n", buffer);
                write(1, "the receievd message is: ", 25);
                write(1, buffer, strlen(buffer));
                write(1, " \n", 2);
                //printf("Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n");
                if (write_desc_flag)
                {
                    write(1, "Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n",
                     102);
                    write_desc_flag = 0;
                }
                if (strlen(buffer) <= 9)
                {
                    res = split(buffer);
                    port_adr_recieved = atoi(res[0]);
                    client_turn = atoi(res[1]);
                    //printf("received port address is: %d and game turn is : %d \n", port_adr_recieved, client_turn);
                    write(1, "received port address is: ", 26);
                    sprintf(str, "%d", port_adr_recieved);
                    write(1, str, strlen(str));
                    memset(str, 0, sizeof(str));
                    write(1, " and game turn is : ", 20);
                    sprintf(str, "%d", client_turn);
                    write(1, str, strlen(str));
                    memset(str, 0, sizeof(str));
                    write(1, " \n", 2);
                    close(client_fd);
                    break;
                }
            }
        }

        /* else its some IO operation on some other socket */
        if (FD_ISSET(fileno(stdin), &server_fds))
        {
            //printf("Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n");
            //fgets(buffer, 1024, stdin);
            read(0 , buffer, 1024);
            strtok(buffer, "\n");
            //printf("You typed: %s\n", buffer);
            if (send(client_fd, buffer, strlen(buffer), 0) < 0)
            {
                //perror("sending message failed");
                write(2, "sending message failed\n", 23);
            }
            else
            {
                //printf("the sent message : \"%s\" \n", buffer);
                group_members = atoi(buffer);
                write(1, "the sent message : \"", 21);
                write(1, buffer, strlen(buffer));
                write(1, "\" \n", 3);
            }
        }
    }
    /* ---------------------------------------------------------------------*/

    Create_UDP_BroadCasting();
    // getpeername(udp_fd, (struct sockaddr *)&broadcast_addr, (socklen_t *)&broadcast_addr);
    // printf("UDP connected , ip %s , port %d \n",
    //        inet_ntoa(broadcast_addr.sin_addr), ntohs(broadcast_addr.sin_port));
    // game_inti();

    event = 0;
    valread = 0;
    int addr_len, cli_len;

    while (TRUE)
    {
        memset(buffer, 0, sizeof(buffer));
        /* clear the descriptor set */
        FD_ZERO(&games_fds);

        /* set udp_fd in readset */
        FD_SET(udp_fd, &games_fds);

        /* set stdin in readset */
        FD_SET(fileno(stdin), &games_fds);

        /* set max_fds */
        max_udp_fd = udp_fd;

        /* wait for an activity on one of the sockets , timeout is NULL ,
         * so wait indefinitely */
        event = select(max_udp_fd + 1, &games_fds, NULL, NULL, NULL);

        if ((event < 0) && (errno != EINTR))
        {
            //perror("select() failed");
            write(2, "UDP select() failed\n", 20);
        }

        /* If something happened on the server socket ,
         * then its an incoming connection */
        if (FD_ISSET(udp_fd, &games_fds))
        {
            addr_len = sizeof(broadcast_addr);
            cli_len = sizeof(cli_addr);
            if ((valread = recvfrom(udp_fd, buffer, 1024, 0, (struct sockaddr *)&cli_addr, &cli_len)) == 0)
            {
                
                // /* Somebody disconnected , get his details and print */
                // getpeername(client_fd, (struct sockaddr *)&servaddr, (socklen_t *)&servaddr);
                printf("UDP connected , ip %s , port %d \n",
                       inet_ntoa(broadcast_addr.sin_addr), ntohs(broadcast_addr.sin_port));
                // write(1, "Host disconnected , ip : ", 25);
                // write(1, inet_ntoa(servaddr.sin_addr), strlen(inet_ntoa(servaddr.sin_addr)));
                // write(1, " , port ", 8);
                // sprintf(str, "%d", ntohs(servaddr.sin_port));
                // write(1, str, strlen(str));
                // memset(str, 0, sizeof(str));
                // write(1, " \n", 2);

                /* Close the socket */
                close(udp_fd);
            }

            /* Echo back the message that came in */
            else
            {
                /* set the string terminating NULL byte on the end
                * of the data read */
                printf("%s: from %s:UDP%u : %s \n",
                       argv[0], inet_ntoa(cli_addr.sin_addr),
                       ntohs(cli_addr.sin_port), buffer);
                buffer[valread] = '\0';
                //printf("the receievd message is: %s\n", buffer);
                write(1, "the receievd message is: \"", 26);
                write(1, buffer, strlen(buffer));
                write(1, "\" \n", 3);
            }
        }

        /* else its some IO operation on some other socket */
        if (FD_ISSET(fileno(stdin), &games_fds))
        {
            //printf("Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n");
            //fgets(buffer, 1024, stdin);
            read(0, buffer, 1024);
            strtok(buffer, "\n");
            //printf("You typed: %s\n", buffer);
            //send(udp_fd, buffer, strlen(buffer), 0)
            if ( sendto(udp_fd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0)
            {
                //perror("sending message failed");
                write(2, "sending message failed\n", 23);
            }
            else
            {
                //printf("the sent message : \"%s\" \n", buffer);
                write(1, "the sent message : \"", 21);
                write(1, buffer, strlen(buffer));
                write(1, "\" \n", 3);
            }
        }
    }

    return 0;
}