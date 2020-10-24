#include <stdio.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

#define BOARD_WIDTH_FOUR 4
#define BOARD_WIDTH_THREE 3
#define BOARD_WIDTH_TWO 2

int port_adr_recieved = 8001;
int client_turn = 1;
int group_members = 3;

int mapH2[BOARD_WIDTH_TWO][BOARD_WIDTH_TWO + 1]; /* map for */
int mapV2[BOARD_WIDTH_TWO + 1][BOARD_WIDTH_TWO]; /*   two   */
int scores2[BOARD_WIDTH_TWO][BOARD_WIDTH_TWO];   /* players */

int mapH3[BOARD_WIDTH_THREE][BOARD_WIDTH_THREE + 1]; /* map for */
int mapV3[BOARD_WIDTH_THREE + 1][BOARD_WIDTH_THREE]; /*  three  */
int scores3[BOARD_WIDTH_THREE][BOARD_WIDTH_THREE];   /* players */

int mapH4[BOARD_WIDTH_FOUR][BOARD_WIDTH_FOUR + 1]; /* map for */
int mapV4[BOARD_WIDTH_FOUR + 1][BOARD_WIDTH_FOUR]; /*   four  */
int scores4[BOARD_WIDTH_FOUR][BOARD_WIDTH_FOUR];   /* players */

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

int main()
{   
    int num_of_playes = group_members;

    int turn = client_turn;

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

    while (TRUE)
    {
        if (group_members == 2)
        {
            while (!Have_Winner(2, scores2))
            {
                Print_Map(2, mapV2, mapH2, scores2);
                printf("turn : Player-%d. Please enter coordinates : \n", turn);
                int z, x, y;
                scanf("%d %d %d", &z, &x, &y);
                turn = Add_New_Line(2, z, x, y, mapV2, mapH2, scores2, turn);
                int i, j;
                for (i = 0; i < group_members + 1; i++)
                {
                    for (j = 0; j < group_members + 1; j++)
                    {
                        printf("%d ", scores2[i][j]);
                    }
                    printf("\n");
                }
            }
            Print_Map(2, mapV2, mapH2, scores2);
            printScores(2, scores2);
            break;
        }
        else if (group_members == 3)
        {
            while (!Have_Winner(3, scores3))
            {
                Print_Map(3, mapV3, mapH3, scores3);
                printf("turn : Player-%d. Please enter coordinates : \n", turn);
                int z, x, y;
                scanf("%d %d %d", &z, &x, &y);
                turn = Add_New_Line(3, z, x, y, mapV3, mapH3, scores3, turn);
                int i, j;
                for (i = 0; i < group_members + 1; i++)
                {
                    for (j = 0; j < group_members + 1; j++)
                    {
                        printf("%d ", scores3[i][j]);
                    }
                    printf("\n");
                }
            }
            Print_Map(3, mapV3, mapH3, scores3);
            printScores(3, scores3);
            break;
        }
        else if (group_members == 4)
        {
            while (!Have_Winner(4, scores4))
            {
                Print_Map(4, mapV4, mapH4, scores4);
                printf("turn : Player-%d. Please enter coordinates : \n", turn);
                int z, x, y;
                scanf("%d %d %d", &z, &x, &y);
                turn = Add_New_Line(4, z, x, y, mapV4, mapH4, scores4, turn);
                int i, j;
                for (i = 0; i < group_members + 1; i++)
                {
                    for (j = 0; j < group_members + 1; j++)
                    {
                        printf("%d ", scores4[i][j]);
                    }
                    printf("\n");
                }
            }
            Print_Map(4, mapV4, mapH4, scores4);
            printScores(4, scores4);
            break;
        }
    }
    return 0;
}




