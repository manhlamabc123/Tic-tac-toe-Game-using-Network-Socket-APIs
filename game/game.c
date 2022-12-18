#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"

/*int board[25] = {
       :,:,:,:,:,
       :,-,-,-,:,
       :,-,-,-,:,
       :,-,-,-,:,
       :,:,:,:,:
};*/

const int InMiddle = 4;
const int Corners[4] = {0, 2, 6, 8};
const int Direction[4] = {1, 5, 4, 6};

const int ConvertTo25[9] = {
    6, 7, 8,
    11, 12, 13,
    16, 17, 18};

int GetNumForDir(int startSq, const int dir, const int *board, const int us)
{
    int found = 0;
    while (board[startSq] != BORDER)
    {
        if (board[startSq] != us)
        {
            break;
        }
        found++;
        startSq += dir;
    }
    return found;
}

int FindThreeInARow(const int *board, const int ourindex, const int us)
{
    int Dir = 0, i = 0;
    int threeCount = 1;
    for (i = 0; i < 4; ++i)
    {
        Dir = Direction[i];
        threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
        threeCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
        if (threeCount == 3)
        {
            break;
        }
        threeCount = 1;
    }
    return threeCount;
}

void InitialiseBoard(int *board)
{
    int i = 0;
    for (i = 0; i < 25; ++i)
    {
        board[i] = BORDER;
    }
    for (i = 0; i < 9; ++i)
    {
        board[ConvertTo25[i]] = EMPTY;
    }
}

void PrintBoard(const int *board)
{
    int i = 0;
    char pceChars[] = "OX|-";
    printf("\n\nBoard:\n\n");
    for (i = 0; i < 9; ++i)
    {
        if (i != 0 && i % 3 == 0)
        {
            printf("\n\n");
        }
        printf("%4c", pceChars[board[ConvertTo25[i]]]);
    }
}

int HasEmpty(const int *board)
{
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
            return 1;
    }
    return 0;
}

int GetHumanMove(const int *board, const int Side)
{
    char userInput[4];
    int i = 0;
    int moveOk = 0;
    int move = -1;
    while (moveOk == 0)
    {
        printf("\nPlease enter a move from 1 to 9:\n");
        fgets(userInput, 3, stdin);
        fflush(stdin);

        if (strlen(userInput) != 2)
        {
            printf("Invalid strlen()\n");
            continue;
        }
        if (sscanf(userInput, "%d", &move) != 1)
        {
            move = -1;
            printf("Invalid sscanf()\n");
            continue;
        }

        if (move < 1 || move > 9)
        {
            move = -1;
            printf("Invalid range\n");
            continue;
        }

        move--;
        if (board[ConvertTo25[move]] != EMPTY)
        {
            move = -1;
            printf("Square not available\n");
            continue;
        }
        moveOk = 1;
    }
    printf("Making Move..%d\n", (move + 1));
    return ConvertTo25[move];
}

int GetNextBest(const int *board)
{
    int ourMove = ConvertTo25[InMiddle];
    if (board[ourMove] == EMPTY)
    {
        return ourMove;
    }
    ourMove = -1;
    int i = 0;
    for (i = 0; i < 4; ++i)
    {
        ourMove = ConvertTo25[Corners[i]];
        if (board[ourMove] == EMPTY)
        {
            break;
        }
        ourMove = -1;
    }
    return ourMove;
}

int GetWinningMove(int *board, const int side)
{
    int ourMove = -1;
    int winMove = 0;
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
        {
            ourMove = ConvertTo25[i];
            board[ourMove] = side;

            if (FindThreeInARow(board, ourMove, side) == 3)
            {
                winMove = 1;
            }
            board[ourMove] = EMPTY;
            if (winMove == 1)
            {
                return ourMove;
            }
            ourMove = -1;
        }
    }
    return ourMove;
}

int GetComputerMove(int *board, const int side)
{
    int numFree = 0;
    int availableMoves[9];
    int randMove = 0;

    randMove = GetWinningMove(board, side);
    if (randMove != -1)
    {
        return randMove;
    }

    randMove = GetWinningMove(board, side ^ 1);
    if (randMove != -1)
    {
        return randMove;
    }

    randMove = GetNextBest(board);
    if (randMove != -1)
    {
        return randMove;
    }
    randMove = 0;
    /* availableMoves[0]
     */
    int i = 0;
    for (i = 0; i < 9; ++i)
    {
        if (board[ConvertTo25[i]] == EMPTY)
        {
            availableMoves[numFree++] = ConvertTo25[i];
        }
    }
    randMove = (rand() % numFree);
    return availableMoves[randMove];
}

void MakeMove(int *board, const int sq, const int side)
{
    board[sq] = side;
}

void RunGame()
{
    int board[25];
    int GameOver = 0;
    int Side = NOUGHTS;
    int LastMoveMade = 0;

    InitialiseBoard(&board[0]);
    PrintBoard(&board[0]);

    while (!GameOver)
    {
        if (Side == NOUGHTS)
        {
            LastMoveMade = GetHumanMove(&board[0], Side);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = CROSSES;
        }
        else
        {
            LastMoveMade = GetComputerMove(&board[0], Side);
            MakeMove(&board[0], LastMoveMade, Side);
            Side = NOUGHTS;
            PrintBoard(&board[0]);
        }

        if (FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3)
        {
            printf("\nGame over!\n");
            GameOver = 1;
            if (Side == NOUGHTS)
            {
                printf("Computer wins\n");
            }
            else
            {
                printf("Human wins\n");
            }
        }
        if (!HasEmpty(board))
        {
            printf("Game over!\n");
            GameOver = 1;
            printf("It's a draw!\n");
        }
    }
    PrintBoard(&board[0]);
}