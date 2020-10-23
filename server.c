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

char *wait_message = "wait for finding opponent(s).";
int client_giving_port = 8001;
char *status[MAX_CLIENTS_COUNT];
/* ------------------ */


void Request_Handler(char *info, int index)
{
    printf("message get from client in Request_Handler : \"%s\"\n", info);
    if (strcmp(info, "2") == 0)
    {
        status[index] = "2";
    }
    else if (strcmp(info, "3") == 0)
    {
        status[index] = "3";
    }
    else if (strcmp(info, "4") == 0)
    {
        status[index] = "4";
    }
    printf("Client requested a %s-Group\n", status[index]);
}

void Connect_To_1_Opponent(int index, int i)
{
    perror("get inside Connect_To_1_Opponent() function");
    int port;
    int data_send1, data_send2;
    char str_port[5];
    char turn1[2];
    char turn2[2];
    char wspace[2];
    char sending_message1[10];
    char sending_message2[10];

    port = client_giving_port;
    client_giving_port++;
    perror("before sprintf() func");
    sprintf(str_port, "%d", port);

    perror("before strcpy() func");

    strcpy(turn1, "1");
    strcpy(turn2, "2");
    strcpy(wspace, " ");

    perror("before first strcat() func");

    strcat(sending_message1, str_port);
    strcat(sending_message1, wspace);
    strcat(sending_message1, turn1);
    int len1 = strlen(sending_message1);

    perror("before second strcat() func");

    strcat(sending_message2, str_port);
    strcat(sending_message2, wspace);
    strcat(sending_message2, turn2);
    int len2 = strlen(sending_message2);

    perror("being before send() function");
    printf("sending_message1 is : \"%s\" and len1 is : %d on fd : %d\n", sending_message1, len1, clientSockets[index]);
    printf("sending_message2 is : \"%s\" and len2 is : %d on fd : %d and i = %d\n", sending_message2, len2, clientSockets[i], i);
    data_send2 = sendto(clientSockets[i], sending_message2, len2, 0, NULL, 0);
    data_send1 = send(clientSockets[index], sending_message1, len1, 0);
    if (data_send2 < 0 && data_send1 < 0)
    {
        perror("sending opponent data failed");
    }
    else
    {
        printf("data_send1 is : \"%d\" and data_send2 is : %d\n", data_send1, data_send2);
        printf("two clients are connecting...\n");
        close(clientSockets[i]);
        close(clientSockets[index]);
        clientSockets[i] = 0;
        clientSockets[index] = 0;
    }
}

void Connect_To_2_Opponent(int index, int i, int j)
{
    int port;
    char *str_port;
    char *turn1;
    char *turn2;
    char *turn3;
    char *wspace;
    char *sending_message1 = "";
    char *sending_message2 = "";
    char *sending_message3 = "";

    port = client_giving_port;
    client_giving_port++;
    perror("before sprintf() func");
    sprintf(str_port, "%d", port);

    strcpy(turn1, "1");
    strcpy(turn2, "2");
    strcpy(turn3, "3");
    strcpy(wspace, " ");

    strcat(sending_message1, str_port);
    strcat(sending_message1, wspace);
    strcat(sending_message1, turn1);
    int len1 = strlen(sending_message1);

    strcat(sending_message2, str_port);
    strcat(sending_message2, wspace);
    strcat(sending_message2, turn2);
    int len2 = strlen(sending_message2);

    strcat(sending_message3, str_port);
    strcat(sending_message3, wspace);
    strcat(sending_message3, turn3);
    int len3 = strlen(sending_message3);

    if (send(clientSockets[index], sending_message1, len1, 0) < 0 && send(clientSockets[i], sending_message2, len2, 0) < 0 && send(clientSockets[j], sending_message3, len3, 0) < 0)
    {
        perror("sending opponent data failed");
    }
    else
    {
        printf("three clients are connecting...\n");
        close(clientSockets[i]);
        close(clientSockets[j]);
        close(clientSockets[index]);
        clientSockets[i] = 0;
        clientSockets[j] = 0;
        clientSockets[index] = 0;
    }
}

void Connect_To_3_Opponent(int index, int i, int j, int k)
{
    int port;
    char *str_port;
    char *turn1;
    char *turn2;
    char *turn3;
    char *turn4;
    char *wspace;
    char *sending_message1 = "";
    char *sending_message2 = "";
    char *sending_message3 = "";
    char *sending_message4 = "";

    port = client_giving_port;
    client_giving_port++;
    perror("before sprintf() func");
    sprintf(str_port, "%d", port);

    strcpy(turn1, "1");
    strcpy(turn2, "2");
    strcpy(turn3, "3");
    strcpy(turn4, "4");
    strcpy(wspace, " ");

    strcat(sending_message1, str_port);
    strcat(sending_message1, wspace);
    strcat(sending_message1, turn1);
    int len1 = strlen(sending_message1);

    strcat(sending_message2, str_port);
    strcat(sending_message2, wspace);
    strcat(sending_message2, turn2);
    int len2 = strlen(sending_message2);

    strcat(sending_message3, str_port);
    strcat(sending_message3, wspace);
    strcat(sending_message3, turn3);
    int len3 = strlen(sending_message3);

    strcat(sending_message4, str_port);
    strcat(sending_message4, wspace);
    strcat(sending_message4, turn4);
    int len4 = strlen(sending_message4);

    if (send(clientSockets[index], sending_message1, len1, 0) < 0 && send(clientSockets[i], sending_message2, len2, 0) < 0 && send(clientSockets[j], sending_message3, len3, 0) < 0 && send(clientSockets[k], sending_message4, len4, 0) < 0)
    {
        perror("sending opponent data failed");
    }
    else
    {
        printf("four clients are connecting...\n");
        close(clientSockets[i]);
        close(clientSockets[j]);
        close(clientSockets[k]);
        close(clientSockets[index]);
        clientSockets[i] = 0;
        clientSockets[j] = 0;
        clientSockets[k] = 0;
        clientSockets[index] = 0;
    }
}

void Find_Opponent(int index)
{
    //printf("Find_Opponent opened \n");
    int len_wait_mes = strlen(wait_message) + 1;
    int num;
    num = atoi(status[index]);
    printf("wants to go inside the switch case \n");
    printf("num is : \"%d\" \n", num);
    if (num == 2)
    {
        printf("in num == 2 \n");
        for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        {
            if (i == index)
                continue;
            else
            {
                if (clientSockets[i] != 0 && strcmp(status[i], status[index]) == 0)
                {
                    //printf("find a copponent !!!! \n");
                    Connect_To_1_Opponent(index, i);
                    printf("-------- start a 2-person game ------- \n");
                    return;
                }
            }
        }

        if (send(clientSockets[index], wait_message, len_wait_mes, 0) < 0)
        {
            perror("sending find opponent message failed");
        }
        printf("the sent mwssage : %s \n", wait_message);
    }

    else if (num == 3)
    {
        printf("in num == 3 \n");
        for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        {
            if (i == index)
                continue;
            else
            {
                if (clientSockets[i] != 0 && strcmp(status[i], status[index]) == 0)

                    for (int j = 0; j < MAX_CLIENTS_COUNT; j++)
                    {
                        if (j == index || j == i)
                            continue;
                        else
                        {
                            if (clientSockets[j] != 0 && strcmp(status[j], status[index]) == 0)
                            {
                                Connect_To_2_Opponent(index, i, j);
                                printf("-------- start a 3-person game ------- \n");
                                return;
                            }
                        }
                    }
            }
        }

        if (send(clientSockets[index], wait_message, len_wait_mes, 0) < 0)
        {
            perror("sending find opponent message failed");
        }
        printf("the sent mwssage : %s \n", wait_message);
    }
    else if (num == 4)
    {
        printf("in num == 4 \n");
        for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        {
            if (i == index)
                continue;
            else
            {
                if (clientSockets[i] != 0 && strcmp(status[i], status[index]) == 0)

                    for (int j = 0; j < MAX_CLIENTS_COUNT; j++)
                    {
                        if (j == index || j == i)
                            continue;
                        else
                        {
                            if (clientSockets[j] != 0 && strcmp(status[j], status[index]) == 0)

                                for (int k = 0; k < MAX_CLIENTS_COUNT; k++)
                                {
                                    if (k == index || k == i || k == j)
                                        continue;
                                    else
                                    {
                                        if (clientSockets[k] != 0 && strcmp(status[k], status[index]) == 0)
                                        {
                                            Connect_To_3_Opponent(index, i, j, k);
                                            printf("-------- start a 4-person game ------- \n");
                                            return;
                                        }
                                    }
                                }
                        }
                    }
            }
        }

        if (send(clientSockets[index], wait_message, len_wait_mes, 0) < 0)
        {
            perror("sending find opponent message failed");
        }
    }
}

///////////////////////////

void Create_TCP_Server_Port()
{
    int opt = 1;
    /* creating socket with IPv4 protocol and TCP type */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        //perror("socket failed");
        write(2, "socket failed\n", 14);
        exit(EXIT_FAILURE);
    }

    /*  set master socket to allow multiple connections */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        //perror("setsockopt failed");
        write(2, "setsockopt failed\n", 18);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;       // for IPv4 protocol
    address.sin_addr.s_addr = INADDR_ANY; // to bind the server to the localhost "127.0.0.1"
    address.sin_port = htons(SERV_PORT);  // host format to network format convertor

    /* binding socket to the port [8080] in localhost*/
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        //perror("bind failed");
        write(2, "bind failed\n", 12);
        exit(EXIT_FAILURE);
    }

    /* listening */
    if (listen(server_fd, LISTENQ) < 0)
    {
        //perror("listening failed");
        write(2, "listening failed\n", 17);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    int event, valread;
    int sd;
    Create_TCP_Server_Port();

    char buffer[MAXSIZE+1]; //data buffer of 1K
    char buff[MAXSIZE + 1];
    char str[MAXSIZE + 1];

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
            //perror("select() failed");
            write(2, "select() failed\n", 16);
        }

        /* If something happened on the master socket ,
         * then its an incoming connection */
        if (FD_ISSET(server_fd, &client_fds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                //perror("accepting faild");
                write(2, "accepting faild\n", 16);
                exit(EXIT_FAILURE);
            }

            /* inform user of socket number - used in send and receive commands */
            //printf("New connection , socket fd is %d , ip is : %s , port : %d\n " , new_socket ,
            //    inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            write(1, "New connection , socket fd is ", 30);
            sprintf(str, "%d", new_socket);
            write(1, str, strlen(str));
            memset(str, 0, sizeof(str));
            write(1, " , ip is : ", 11);
            write(1, inet_ntoa(address.sin_addr), strlen(inet_ntoa(address.sin_addr)));
            write(1, " , port ", 8);
            sprintf(str, "%d", ntohs(address.sin_port));
            write(1, str, strlen(str));
            memset(str, 0, sizeof(str));
            write(1, " \n", 2);

            /* send new connection greeting message */
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
            {
                //perror("Welcome message not sent !");
                write(2, "Welcome message not sent !\n", 27);
            }
            else
                //puts("Welcome message sent successfully");
                write(1, "Welcome message sent successfully\n", 34);

            /* add new socket to array of sockets */
            for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
            {
                //if position is empty
                if (clientSockets[i] == 0)
                {
                    clientSockets[i] = new_socket;
                    //printf("Adding to list of sockets as %d\n", i);
                    write(1, "Adding to list of sockets as ", 29);
                    sprintf(str, "%d", i);
                    write(1, str, strlen(str));
                    memset(str, 0, sizeof(str));
                    write(1, "\n", 1);
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
                    //printf("Host disconnected , ip %s , port %d \n",
                    //    inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    write(1, "Host disconnected , ip : ", 25);
                    write(1, inet_ntoa(address.sin_addr), strlen(inet_ntoa(address.sin_addr)));
                    write(1, " , port ", 8);
                    sprintf(str, "%d", ntohs(address.sin_port));
                    write(1, str, strlen(str));
                    memset(str, 0, sizeof(str));
                    write(1, " \n", 2);

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
                    //send(sd, buffer, strlen(buffer), 0);
                    //printf("the received message is: \"%s\" \n", buffer);
                    write(1, "the received message is: \"", 26);
                    write(1, buffer, strlen(buffer));
                    //write(1, buffer, strlen(buffer) - 2);  for telnet command
                    write(1, "\" \n", 3);
                    Request_Handler(buffer, i);
                    Find_Opponent(i);
                }
            }
        }
    }
    return 0;
}

// printf("Enter the number of group members from one of [2] or [3] or [4] group members :(type only the number)\n");
// fgets(buff, 1024, stdin);
// strtok(buff, "\n");
// printf("You typed: %s\n", buff);
// if (send(sd, buff, strlen(buff), 0) < 0)
// {
//     perror("sending message failed");
// }
// else
//     printf("the sent message : \"%s\" \n", buffer);

