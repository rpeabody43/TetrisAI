#include "Agent.h"

Agent::Agent (bool hardDrop, Weights weights)
    : m_weights(weights)
      , m_workingMove({})
      , m_currentPieceNum(14)
      , m_fitness(0)
      , m_hardDrop(hardDrop)
{}

Input Agent::GenInput (Board* currentBoard)
{
    Input input = {};
    if (m_currentPieceNum != currentBoard->GetPieceNum())
    {
        m_workingMove = BestMove(currentBoard, m_weights);
        m_currentPieceNum = currentBoard->GetPieceNum();
    }

    if (m_workingMove.hold)
    {
        input.holdPiece = true;
        m_workingMove.hold = false;
        return input;
    }

    uint8_t currentPiece = currentBoard->GetFallingPiece();
    uint8_t currentRot = currentBoard->GetFallingPieceRot();

    int8_t rotDelta = m_workingMove.rotation - currentRot;
    if (rotDelta < 0)
    {
        input.rotCountClockwise = true;
    }
    else if (rotDelta > 0)
    {
        input.rotClockwise = true;
    }

    uint16_t currentAnchor = currentBoard->GetFallingPieceAnchor();
    // int8_t rowDelta = Board::Row(m_workingMove.position) - Board::Row(currentAnchor);
    int8_t offset = TetrominoData::GetPieceBounds(currentPiece, currentRot).leftBound;

    uint8_t targetCol = Board::Col(m_workingMove.position - offset);
    uint8_t currentCol = Board::Col(currentAnchor - offset);
    int8_t colDelta = targetCol - currentCol;
    if (colDelta < 0)
    {
        input.moveLeft = true;
    }
    else if (colDelta > 0)
    {
        input.moveRight = true;
    }
    else
    {
        if (m_hardDrop)
        {
            if (rotDelta == 0)
            {
                input.hardDrop = true;
            }
        }
        else
        {
            input.softDrop = true;
        }
    }


    return input;
}