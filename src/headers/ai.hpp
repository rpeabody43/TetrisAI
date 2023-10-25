#include "Board.hpp"
#include "tetrominoes.hpp"

struct Move {
    int position;
    int rotation;
    bool hold;
};

Move BestMove(Board* currentBoard, int currentPiece, int heldPiece, int highestPoint);