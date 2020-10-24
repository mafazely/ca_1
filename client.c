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

//  ----------------  game part ----------------- //
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
// ---------------------------------------------- //

int write_desc_flag = 1; //flag for just one time print the description

/* declaring variables*/
int client_fd  ;   // TCP socket
struct sockaddr_in servaddr ; // for tcp connection
struct sockaddr_in broadcast_addr; // for UDP BroadCasting
struct sockaddr_in cli_addr;
struct sockaddr_in bind_addr;

fd_set server_fds; // communication between client and server
int max_fd;

int port_adr_recieved;
int client_turn;
int group_members;

int udp_fd; // UDP socket
fd_set games_fds; // communication between clients
int max_udp_fd;
/* ------------------ */

// ------------------- game part -------------------- //

void Init_Game(int const size, int mapV[size + 1][size], int mapH[size][size + 1], int score[size][size])
{
    for (int i = 0; i < size + 1; i++)
    {
        for (int j = 0; j < size + 1; j++)
        {
            if (j < size)
                mapV[i][j] = 0;
            if (i < size)
                mapH[i][j] = 0;
            if (j < size && j < size)
                score[i][j] = 0;
        }
    }
}

bool Have_Winner(int const size, int scores[size][size])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (scores[i][j] == 0)
                return false;
        }
    }
    return true;
}

void printScores(int const size, int score[size][size])
{
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            if (score[i][j] == 1)
                a++;
            else if (score[i][j] == 2)
                b++;
            else if (score[i][j] == 3)
                c++;
            else if (score[i][j] == 4)
                d++;
        }

    switch (size)
    {
    case 2:
        printf("Player 1 : %d\n", a);
        printf("Player 2 : %d\n", b);
        printf("Player-%s won the game!", a > b ? "1" : "2");
        break;

    case 3:
        printf("Player 1 : %d\n", a);
        printf("Player 2 : %d\n", b);
        printf("Player 3 : %d\n", c);

        if (a > b)
        {
            if (a > c)
                printf("Player-%s won the game!", "1");
            else
                printf("Player-%s won the game!", "3");
        }
        else
            printf("Player-%s won the game!", "2");
        break;

    case 4:
        printf("Player 1 : %d\n", a);
        printf("Player 2 : %d\n", b);
        printf("Player 3 : %d\n", c);
        printf("Player 4 : %d\n", d);

        if (a > b)
        {
            if (a > c)
            {
                if (a > d)
                    printf("Player-%s won the game!", "1");
                else
                    printf("Player-%s won the game!", "4");
            }
            else
                printf("Player-%s won the game!", "3");
        }
        else
            printf("Player-%s won the game!", "2");
        break;

    default:
        break;
    }
}

void Util_Vertical_Lines(int const size, int mapV[size + 1][size], int line)
{
    for (int i = 0; i < size; i++)
    {
        if (mapV[line][i] == 1)
        {
            if (i == 0)
                printf("*--*");
            else
                printf("--*");
        }
        else
        {
            if (i == 0)
                printf("*  *");
            else
                printf("  *");
        }
    }
    printf("\n");
}

void Util_Horizonal_Lines(int const size, int mapH[size][size + 1], int score[size][size], int line)
{
    for (int i = 0; i < size + 1; i++)
    {
        if (mapH[line][i] == 1)
            printf("|");
        else
            printf(" ");
        if (i != size)
        {
            if (score[line][i] == 4)
                printf("D ");
            else if (score[line][i] == 3)
                printf("C ");
            else if (score[line][i] == 2)
                printf("B ");
            else if (score[line][i] == 1)
                printf("A ");
            else
                printf("  ");
        }
    }
    printf("\n");
}

void Print_Map(int const size, int mapV[size + 1][size], int mapH[size][size + 1], int score[size][size])
{
    printf("\n");
    for (int i = 0; i < size + 1; i++)
    {
        Util_Vertical_Lines(size, mapV, i);
        if (i != size)
            Util_Horizonal_Lines(size, mapH, score, i);
    }
}

int Add_New_Line(int const size, int z, int x, int y, int mapV[size + 1][size], int mapH[size][size + 1], int score[size][size], int turn)
{
    int s, newTurn;
    int local_turn = turn - 1;

    if (size == 2)
    {
        // if (local_turn % 2 == 0)
        //     local_turn++;
        newTurn = (local_turn + 1) % 2;
    }
    else if (size == 3)
    {
        // if (local_turn % 3 == 0)
        //     local_turn++;
        newTurn = (local_turn + 1) % 3;
    }
    else if (size == 4)
    {
        // if (local_turn % 4 == 0)
        //     local_turn++;
        newTurn = (local_turn + 1) % 4;
    }

    if (local_turn == 1)
        s = 1;
    else if (local_turn == 2)
        s = 2;
    else if (local_turn == 3)
        s = 3;
    else if (local_turn == 4)
        s = 4;

    if (z == 0)
    {
        mapV[x - 1][y - 1] = 1;
        if (x != 1)
            if (mapH[x - 2][y - 1] && mapH[x - 2][y] && mapV[x - 2][y - 1]) // Check upper square
            {
                score[x - 2][y - 1] = s;
                newTurn = local_turn;
            }
        if (x != size + 1)
            if (mapH[x - 1][y - 1] && mapH[x - 1][y] && mapV[x][y - 1]) // Check lower
            {
                score[x - 1][y - 1] = s;
                newTurn = local_turn;
            }
    }
    if (z == 1)
    {
        mapH[x - 1][y - 1] = 1;
        if (y != 1)
            if (mapV[x - 1][y - 2] && mapV[x][y - 2] && mapH[x - 1][y - 2]) // left square
            {
                score[x - 1][y - 2] = s;
                newTurn = local_turn;
            }
        if (y != size + 2)
            if (mapV[x - 1][y - 1] && mapV[x][y - 2] && mapH[x - 1][y]) // right square
            {
                score[x - 1][y - 1] = s;
                newTurn = local_turn;
            }
    }
    //printf("returnd trun is : %d", local_turn);
    return (newTurn + 1);
}

void first_init_game()
{
    int num_of_playes = group_members;

    if (num_of_playes == 2)
    {
        Init_Game(2, mapV2, mapH2, scores2);
    }
    else if (num_of_playes == 3)
    {
        Init_Game(3, mapV3, mapH3, scores3);
    }
    else if (num_of_playes == 4)
    {
        Init_Game(4, mapV4, mapH4, scores4);
    }
}

void game(int z, int x, int y)
{
    int num_of_playes = group_members;
    int turn = client_turn;
    char str[MAXSIZE + 1];

    if (group_members == 2)
    {
        while (!Have_Winner(2, scores2))
        {
            Print_Map(2, mapV2, mapH2, scores2);
            //printf("turn : Player-%d. Please enter coordinates : \n", turn);
            write(1, "turn : Player-", 14);
            sprintf(str, "%d", turn);
            write(1, str, strlen(str));
            memset(str, 0, sizeof(str));
            write(1, ". Please enter coordinates : \n", 30);
            turn = Add_New_Line(2, z, x, y, mapV2, mapH2, scores2, turn);
        }
        Print_Map(2, mapV2, mapH2, scores2);
        printScores(2, scores2);
    }
    else if (group_members == 3)
    {
        while (!Have_Winner(3, scores3))
        {
            Print_Map(3, mapV3, mapH3, scores3);
            //printf("turn : Player-%d. Please enter coordinates : \n", turn);
            write(1, "turn : Player-", 14);
            sprintf(str, "%d", turn);
            write(1, str, strlen(str));
            memset(str, 0, sizeof(str));
            write(1, ". Please enter coordinates : \n", 30);
            turn = Add_New_Line(3, z, x, y, mapV3, mapH3, scores3, turn);
        }
        Print_Map(3, mapV3, mapH3, scores3);
        printScores(3, scores3);
    }
    else if (group_members == 4)
    {
        while (!Have_Winner(4, scores4))
        {
            Print_Map(4, mapV4, mapH4, scores4);
            //printf("turn : Player-%d. Please enter coordinates : \n", turn);
            write(1, "turn : Player-", 14);
            sprintf(str, "%d", turn);
            write(1, str, strlen(str));
            memset(str, 0, sizeof(str));
            write(1, ". Please enter coordinates : \n", 30);
            turn = Add_New_Line(4, z, x, y, mapV4, mapH4, scores4, turn);
        }
        Print_Map(4, mapV4, mapH4, scores4);
        printScores(4, scores4);
    }
}

// -------------------------------------------------- //


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

    /* Defining socket configures */
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;  // for IPv4 protocol
    bind_addr.sin_addr.s_addr = INADDR_ANY;  // to bind the server to the broadcast ip
    bind_addr.sin_port = htons(port_adr_recieved);  // set recieved port from server

    /* Bind the socket with the server address  */
    if (bind(udp_fd, (const struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0)
    {
        //perror("UDP binding failed");
        write(2, "UDP binding failed\n", 19);
        exit(EXIT_FAILURE);
    }

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    /* Defining socket configures */
    broadcast_addr.sin_family = AF_INET;  // for IPv4 protocol
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP_ADDR);  // for broadcasting
    broadcast_addr.sin_port = htons(port_adr_recieved);  // set recieved port from server
}

int main(int argc, char const *argv[])
{
    char buffer[MAXSIZE+1];
    int event, valread, valread2;
    char **res = NULL;
    char **res_get = NULL;
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

    first_init_game();

    event = 0;
    valread = 0;
    valread2 = 0;
    int addr_len, cli_len;
    int x, y, z;
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
            if ((valread = recv(udp_fd, buffer, 1024, 0)) == 0)
            {
                /* Close the socket */
                close(udp_fd);
            }

            /* Echo back the message that came in */
            else
            {
                /* set the string terminating NULL byte on the end
                * of the data read */
                // printf("%s: from %s:UDP%u : %s \n",
                //        argv[0], inet_ntoa(cli_addr.sin_addr),
                //        ntohs(cli_addr.sin_port), buffer);
                // buffer[valread] = '\0';
                // //printf("the receievd message is: %s\n", buffer);
                // write(1, "the receievd message is: \"", 26);
                // write(1, buffer, strlen(buffer));
                // write(1, "\" \n", 3);
                res_get = split(buffer);
                z = atoi(res_get[1]);
                x = atoi(res_get[2]);
                y = atoi(res_get[3]);
                //game(z, x, y);
            }
        }

        /* else its some IO operation on some other socket */
        if (FD_ISSET(fileno(stdin), &games_fds))
        {
            read(0, buffer, 1024);
            strtok(buffer, "\n");

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