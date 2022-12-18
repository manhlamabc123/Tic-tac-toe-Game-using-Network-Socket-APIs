enum
{
    NOUGHTS,
    CROSSES,
    BORDER,
    EMPTY
};
enum
{
    HUMAN_WIN,
    COMP_WIN,
    DRAW
};

void RunGame();
void MakeMove(int *board, const int sq, const int side);
int GetComputerMove(int *board, const int side);
int GetWinningMove(int *board, const int side);
int GetNextBest(const int *board);
int GetHumanMove(const int *board, const int Side);
int HasEmpty(const int *board);
void PrintBoard(const int *board);
void InitialiseBoard(int *board);