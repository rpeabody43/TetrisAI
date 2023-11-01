// For randomization
#include <algorithm>
#include <random>

#include "Board.h"
#include "tetrominoes.hpp"

uint16_t Board::ConvertIdx (uint8_t x, uint8_t y)
{
    return y * WIDTH + x;
}

uint8_t Board::Row (uint16_t idx)
{
    return idx / WIDTH;
}

uint8_t Board::Col (uint16_t idx)
{
    return idx % WIDTH;
}

Board::Board (uint16_t fallRate, std::default_random_engine& randomGenerator) // NOLINT(*-msc51-cpp)
    : m_gameover(false)
      , m_ticks(0)
      , m_lastTicks(0)
      , m_fallRate(fallRate)
      , m_fallingPiece(0)
      , m_fallingPieceRot(0)
      , m_fallingPieceAnchor(3)
      , m_board{}
      , m_bags{}
      , m_bagIdx(7)
      , m_heldPiece(0)
      , m_alreadyHeld(false)
      , m_currentHighest(HEIGHT)
      , m_score(0)
      , m_linesCleared(0)
      , m_randomGenerator(randomGenerator)
{
    // Initialize each bag in sequential order, then shuffle
    for (auto& bag: m_bags)
    {
        for (int j = 0; j < 7; j++)
            bag[j] = j + 1;
        std::shuffle(std::begin(bag), std::end(bag), m_randomGenerator);
    }
}

uint8_t Board::GetPieceMap (uint8_t rot, uint8_t n) const
{
    uint8_t ret = TetrominoData::GetPieceMap(m_fallingPiece, rot, n);
    return ret;
}

uint8_t Board::NthPiece (uint8_t n) const
{
    uint8_t idx = m_bagIdx + n;
    if (idx >= sizeof(m_bags))
        idx -= sizeof(m_bags);
    return m_bags[idx / 7][idx % 7];
}

uint8_t Board::GetPieceNum () const
{
    return m_bagIdx;
}

void Board::NextPiece ()
{
    // If reached the end of the current piece bag, shuffle it and move onto the next;
    if ((m_bagIdx + 1) % 7 == 0)
    {
        std::shuffle(std::begin(m_bags[m_bagIdx / 7]), std::end(m_bags[m_bagIdx / 7]), m_randomGenerator);
    }
    m_bagIdx++;
    if (m_bagIdx >= sizeof(m_bags))
        m_bagIdx = 0;
}

void Board::NewPiece ()
{
    int piece = NthPiece(0);
    NewPiece(piece);
}


void Board::NewPiece (uint8_t piece)
{
    m_fallingPiece = piece;
    m_fallingPieceRot = 0;
    // If the highest point is just below the vanish zone
    // Spawn the piece in the vanish zone
    if (m_currentHighest <= VANISH_ZONE_HEIGHT + 1)
    {
        m_fallingPieceAnchor = ConvertIdx(3, BUFFER_HEIGHT);
    }
    // Otherwise spawn in visible space
    else
    {
        m_fallingPieceAnchor = ConvertIdx(3, VANISH_ZONE_HEIGHT + BUFFER_HEIGHT);
    }

    // Move up in the bag
    NextPiece();

    uint16_t start = m_fallingPieceAnchor;
    // Pieces spawn on top of other pieces
    bool blockOut = false;

    // Set all falling piece squares to the negative value of the piece
    for (int i = 3; i >= 0; i--)
    {
        uint16_t newIdx = start + GetPieceMap(m_fallingPieceRot, i);

        if (m_board[newIdx] == 0)
        {
            m_board[newIdx] = -m_fallingPiece;
        }
        else blockOut = true;
    }

    m_gameover = blockOut;
}

void Board::HoldPiece ()
{
    if (m_alreadyHeld) return;
    for (int i = 3; i >= 0; i--)
    {
        int idx = m_fallingPieceAnchor + GetPieceMap(m_fallingPieceRot, i);
        m_board[idx] = 0;
    }

    int prevHeldPiece = m_heldPiece;
    m_heldPiece = m_fallingPiece;
    if (prevHeldPiece == 0)
        NewPiece();
    else
        NewPiece(prevHeldPiece);
    m_alreadyHeld = true;
}

void Board::ClearLines ()
{
    const uint16_t start = m_fallingPieceAnchor;
    const uint8_t startRow = Row(start);
    uint8_t linesCleared = 0;

    // Copy lines down to cover cleared lines
    for (int y = startRow; y < std::min(startRow + 5, (int) HEIGHT); y++)
    {
        bool lineComplete = true;
        for (int x = 0; x < 10 && lineComplete; x++)
        {
            if (GetSquare(x, y) == 0)
                lineComplete = false;
        }

        if (lineComplete)
        {
            linesCleared++;

            for (int tempY = y - 1; tempY >= startRow - 1; tempY--)
            {
                for (int tempX = 0; tempX < 10; tempX++)
                {
                    int currentIdx = ConvertIdx(tempX, tempY);
                    int copyIdx = ConvertIdx(tempX, tempY + 1);
                    m_board[copyIdx] = m_board[currentIdx];
                }
            }
        }
    }

    if (linesCleared == 0) return;
    // Copy down the rest of the lines to
    for (int tempY = startRow - 1; tempY >= 0; tempY--)
    {
        for (int tempX = 0; tempX < 10; tempX++)
        {
            uint16_t currentIdx = ConvertIdx(tempX, tempY);
            uint16_t copyIdx = ConvertIdx(tempX, tempY + linesCleared);
            m_board[copyIdx] = m_board[currentIdx];
        }
    }

    // Fill the top with zeroes
    for (int buffer = 0; buffer < ConvertIdx(9, linesCleared); buffer++)
    {
        m_board[buffer] = 0;
    }

    // Since y starts from the top, currentHighest needs to be increased
    m_currentHighest += linesCleared;

    m_linesCleared += linesCleared;
    uint16_t scoreAdd;
    switch (linesCleared)
    {
        default:
            scoreAdd = 0;
            break;
        case 1:
            scoreAdd = 100;
            break;
        case 2:
            scoreAdd = 300;
            break;
        case 3:
            scoreAdd = 500;
            break;
        case 4:
            scoreAdd = 800;
    }
    m_score += scoreAdd*m_linesCleared/10;
}

void Board::Fall ()
{
    m_lastTicks = m_ticks;

    bool freeze = !ValidMove(0, 10);
    if (freeze)
    {
        MovePiece(0, 0, true);
        ClearLines();
        NewPiece();
        return;
    }

    UpdateFallingPiece(0, 10);
}

uint16_t Board::GetGhost ()
{
    return GetGhost(m_fallingPiece, m_fallingPieceAnchor, m_fallingPieceRot);
}

uint16_t Board::GetGhost (uint8_t piece, uint16_t anchor, uint8_t currentRot)
{
    uint16_t delta = 0;
    while (ValidMove(piece, anchor, currentRot, 0, delta + Board::WIDTH))
        delta += Board::WIDTH;
    return anchor + delta;
}

void Board::HardDrop ()
{
    uint16_t moveDelta = GetGhost() - m_fallingPieceAnchor;
    MovePiece(0, moveDelta, true);
    ClearLines();
    NewPiece();
}

bool Board::ValidMove (int8_t rotDelta, int16_t moveDelta)
{
    return ValidMove(m_fallingPiece, m_fallingPieceAnchor, m_fallingPieceRot, rotDelta, moveDelta);
}

bool Board::ValidMove (uint8_t piece, uint16_t anchor, uint8_t currentRot, int8_t rotDelta, int16_t moveDelta)
{
    for (int i = 0; i < 4; i++)
    {
        int oldPos = anchor + TetrominoData::GetPieceMap(piece, currentRot, i);
        int newPos = anchor + moveDelta + TetrominoData::GetPieceMap(piece, currentRot + rotDelta, i);
        // If it's an index error on either side
        if (newPos >= TOTAL_SIZE || newPos < 0)
            return false;
        // If it's a nonempty square, that also isn't the falling piece itself
        // The falling piece is stored as negative numbers
        if (m_board[newPos] != 0 && m_board[newPos] != -m_fallingPiece)
            return false;
        // If it hit the side of the board
        int oldCol = Col(oldPos);
        int newCol = Col(newPos);
        bool closeToEdge = ((oldCol <= 2) && (newCol >= 8)) || ((oldCol >= 8) && (newCol <= 2));
        if (closeToEdge)
            return false;
    }
    return true;
}

void Board::MovePiece (int8_t rotDelta, int16_t moveDelta, bool freeze)
{
    for (int i = 3; i >= 0; i--)
    {
        // Get the block with the delta from the map array
        int absidxOld = m_fallingPieceAnchor + GetPieceMap(m_fallingPieceRot, i);
        m_board[absidxOld] = 0;
    }
    for (int i = 3; i >= 0; i--)
    {
        uint16_t absidxNew = m_fallingPieceAnchor + moveDelta + GetPieceMap(m_fallingPieceRot + rotDelta, i);
        m_board[absidxNew] = freeze ? m_fallingPiece : -m_fallingPiece;
    }

    m_fallingPieceRot += rotDelta;
    m_fallingPieceAnchor += moveDelta;

    if (freeze)
    {
        // The held piece becomes available when the current falling piece is locked
        m_alreadyHeld = false;

        // We use > because y is from top down
        if (m_currentHighest > Row(m_fallingPieceAnchor))
            m_currentHighest = Row(m_fallingPieceAnchor);

        // Lock out
        if (m_fallingPieceAnchor + GetPieceMap(m_fallingPieceRot, 3) < ConvertIdx(0, VANISH_ZONE_HEIGHT))
        {
            m_gameover = true;
        }
    }
}

void Board::UpdateFallingPiece (int8_t rotDelta, int16_t moveDelta)
{
    // Do movement first because we don't want it to stack with wall kicks
    if (ValidMove(0, moveDelta))
        MovePiece(0, moveDelta, false);

    // Make sure m_fallingPieceRot is between 0 and 3
    while (m_fallingPieceRot + rotDelta < 0)
        rotDelta += 4;
    while (m_fallingPieceRot + rotDelta > 3)
        rotDelta -= 4;

    int wallKickTable = GetWallKickIdx(m_fallingPieceRot, m_fallingPieceRot + rotDelta);
    int i = 0;
    // O pieces should not be rotated / wall kicked at all
    if (m_fallingPiece == TetrominoData::O || rotDelta == 0)
        return;
        // Loop through the wall kicks at this rotation until one works, or they all fail
    else
    {
        // The I piece has a different table of wall kicks per SRS
        if (m_fallingPiece == TetrominoData::I)
        {
            while (i < 5 && !ValidMove(rotDelta, TetrominoData::I_WALL_KICKS[wallKickTable][i]))
                i++;
        }
        else
        {
            while (i < 5 && !ValidMove(rotDelta, TetrominoData::WALL_KICKS[wallKickTable][i]))
                i++;
        }
    }

    if (i != 5)
    {
        int8_t wallKick;
        if (m_fallingPiece == TetrominoData::I)
            wallKick = TetrominoData::I_WALL_KICKS[wallKickTable][i];
        else
            wallKick = TetrominoData::WALL_KICKS[wallKickTable][i];
        MovePiece(rotDelta, wallKick, false);
    }
}

uint8_t Board::GetWallKickIdx (uint8_t startRot, uint8_t endRot)
{
    if (startRot == 3 && endRot == 0)
        return 6;
    else if (startRot == 0 && endRot == 3)
        return 7;

    int dir = endRot - startRot;
    int ret = startRot + endRot;
    if (dir == 1)
        ret -= 1;
    return ret;
}

[[maybe_unused]] uint16_t Board::GetFallingPieceAnchor () const
{
    return m_fallingPieceAnchor;
}

void Board::Update (Input& input, uint32_t ticks)
{
    // m_fallingPiece is only assigned 0 at new game
    // every other piece's number is > 0
    if (m_fallingPiece == 0)
    {
        NewPiece();
        return;
    }
    if (m_gameover) return;
    m_ticks = ticks;
    if (m_ticks - m_lastTicks >= m_fallRate)
    {
        Fall();
    }

    int16_t moveDelta = 0;
    int8_t rotDelta = 0;

    if (input.moveLeft)
        moveDelta -= 1;
    if (input.moveRight)
        moveDelta += 1;
    if (input.softDrop)
        Fall();
    if (input.rotClockwise)
        rotDelta += 1;
    if (input.rotCountClockwise)
        rotDelta -= 1;
    if (input.hardDrop)
        HardDrop();
    if (input.holdPiece)
        HoldPiece();

    UpdateFallingPiece(rotDelta, moveDelta);

}

uint8_t Board::GetFallingPiece () const
{
    return m_fallingPiece;
}

uint8_t Board::GetFallingPieceRot () const
{
    return m_fallingPieceRot;
}

int8_t Board::GetSquare (uint8_t x, uint8_t y) const
{
    return m_board[ConvertIdx(x, y)];
}

int8_t Board::GetSquare (uint16_t idx) const
{
    return m_board[idx];
}

uint8_t Board::GetHeldPiece () const
{
    return m_heldPiece;
}

uint8_t Board::GetHighestRow () const
{
    return m_currentHighest;
}

bool Board::GameOver () const
{
    return m_gameover;
}

size_t Board::GetScore () const
{
    return m_score;
}

size_t Board::GetLinesCleared () const
{
    return m_linesCleared;
}