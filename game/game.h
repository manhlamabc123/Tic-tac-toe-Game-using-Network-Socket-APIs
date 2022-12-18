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

const int InMiddle = 4;
const int Corners[4] = {0, 2, 6, 8};
const int Direction[4] = {1, 5, 4, 6};

const int ConvertTo25[9] = {
    6, 7, 8,
    11, 12, 13,
    16, 17, 18};

void RunGame();
void MakeMove(int *board, const int sq, const int side);
int GetComputerMove(int *board, const int side);
int GetWinningMove(int *board, const int side);
int GetNextBest(const int *board);
int GetHumanMove(const int *board, const int Side);
int HasEmpty(const int *board);
void PrintBoard(const int *board);
void InitialiseBoard(int *board);