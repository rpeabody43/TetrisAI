#include "../game/Board.h"

struct Move
{
    int position;
    int rotation;
    bool hold;
};

Move BestMove(Board* currentBoard, int currentPiece, int heldPiece, int highestPoint);