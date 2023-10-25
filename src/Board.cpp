#include <algorithm> // For randomization

#include "headers/tetrominoes.hpp"
#include "headers/Board.hpp"

Board::Board(int fallRate)
	: m_gameover(false)
	, m_ticks(0)
	, m_lastTicks(0)
	, m_fallingPiece(0)
	, m_fallingPieceRot(0)
	, m_fallingPieceIdx(3)
	, m_board()
	, m_bags()
	, m_bagIdx(7)
	, m_heldPiece(0)
	, m_alreadyHeld(false)
	, m_currentHighest(Board::HEIGHT) 
{
	m_fallRate = fallRate;
	for (int i = 0; i < sizeof(m_bags) / (sizeof(int) * 7); i++)
	{
		for (int j = 0; j < 7; j++)
			m_bags[i][j] = j + 1;
		std::random_shuffle(std::begin(m_bags[i]), std::end(m_bags[i]));
	}
}

int Board::GetPieceMap(int rot, int idx)
{
	int ret = TetrominoData::GetPieceMap(m_fallingPiece, rot, idx);
	return ret;
}

int Board::NthPiece(int delta)
{
	int idx = m_bagIdx + delta;
	if (idx >= sizeof(m_bags) / sizeof(int))
		idx -= sizeof(m_bags) / sizeof(int);
	return m_bags[idx / 7][idx % 7];
}

void Board::NextPiece () 
{
	// If reached the end of the current piece bag, shuffle it and and move onto the next;
	if ((m_bagIdx + 1) % 7 == 0)
	{
		std::random_shuffle(std::begin(m_bags[m_bagIdx / 7]), std::end(m_bags[m_bagIdx / 7]));
	}
	m_bagIdx++;
	if (m_bagIdx >= sizeof(m_bags) / sizeof(int))
		m_bagIdx = 0;
}

void Board::NewPiece()
{
	int piece = NthPiece(0);
	NewPiece(piece);
}


void Board::NewPiece(int piece)
{
	m_fallingPiece = piece;
    // Point the piece starts drawing from
	if (m_currentHighest <= VANISH_ZONE_HEIGHT + 1)
		m_fallingPieceIdx = 3; 
	else
        m_fallingPieceIdx = IdxConvert(3, VANISH_ZONE_HEIGHT);
	m_fallingPieceRot = 0;

	NextPiece();

	int start = m_fallingPieceIdx;
	// Pieces spawn on top of other pieces
	bool blockOut = false;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + GetPieceMap(m_fallingPieceRot, i);

		if (m_board[newIdx] == 0)
		{
			m_board[newIdx] = -m_fallingPiece;
		}
		else blockOut = true;
	}

	//m_gameover = blockOut;
}

void Board::HoldPiece()
{
	if (m_alreadyHeld) return;
	for (int i = 3; i >= 0; i--)
	{
		int idx = m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, i);
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

void Board::ClearLines()
{
	const int start = m_fallingPieceIdx;
	const int startRow = Row(start);
	const int startCol = Col(start);
	int linesCleared = 0;

	for (int y = startRow; y < std::min(startRow+5, HEIGHT); y++)
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
					int currentIdx = IdxConvert(tempX, tempY);
					int copyIdx = IdxConvert(tempX, tempY + 1);
					m_board[copyIdx] = m_board[currentIdx];
				}
			}
		}
	}

	if (linesCleared == 0) return;
	for (int tempY = startRow - 1; tempY >= 0; tempY--)
	{
		for (int tempX = 0; tempX < 10; tempX++)
		{
			int currentIdx = IdxConvert(tempX, tempY);
			int copyIdx = IdxConvert(tempX, tempY + linesCleared);
			m_board[copyIdx] = m_board[currentIdx];
		}
	}

	for (int buffer = 0; buffer < IdxConvert(9, linesCleared); buffer++)
	{
		m_board[buffer] = 0;
	}

	m_currentHighest += linesCleared;
}

void Board::Fall()
{
	m_lastTicks = m_ticks;
	if (m_fallingPiece == 0)
	{
		NewPiece();
		return;
	}

	bool placeDownImmediately = !ValidMove(0, 10);
	if (placeDownImmediately)
	{
		MovePiece(0, 0, true);
		ClearLines();
		NewPiece();
		return;
	}

	UpdateFallingPiece(0, 10);
}

int Board::GetFallingPiece()
{
	return m_fallingPiece;
}

int Board::GetFallingPieceRot()
{
	return m_fallingPieceRot;
}

int Board::GetGhost()
{
	int delta = 0;
	while (ValidMove(0, delta + 10))
		delta += 10;
	return m_fallingPieceIdx + delta;
}

void Board::HardDrop()
{
	int ghost = GetGhost() - m_fallingPieceIdx;
	MovePiece(0, ghost, true);
	ClearLines();
	NewPiece();
}

bool Board::ValidMove(int rotDelta, int moveDelta)
{
	for (int i = 0; i < 4; i++)
	{
		int oldPos = m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, i);
		int newPos = m_fallingPieceIdx + moveDelta + GetPieceMap(m_fallingPieceRot+rotDelta, i);
		// If it's an index error on either side
		if (newPos >= HEIGHT * WIDTH || newPos < 0)
			return false;
		// If it's a nonempty square, that also isn't the falling piece itself
		if (m_board[newPos] != 0 && m_board[newPos] != -m_fallingPiece)
			return false;
		// If it would hit the side of the board
		int oldCol = Col(oldPos);
		int newCol = Col(newPos);
		bool closeToEdge = ((oldCol <= 2) && (newCol >= 8)) || ((oldCol >= 8) && (newCol <= 2));
		if (closeToEdge)
			return false;
	}
	return true;
}

// DOESN'T DO ANY CHECKS
// JUST MOVES PIECE AND LEAVES ZEROES IN ITS PLACE
void Board::MovePiece(int rotDelta, int moveDelta, bool freeze)
{
	for (int i = 3; i >= 0; i--)
	{
		// Get the block with the delta from the map array
		int absidxOld = m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, i);
		m_board[absidxOld] = 0;
	}
	for (int i = 3; i >= 0; i--)
	{
		int absidxNew = m_fallingPieceIdx + moveDelta + GetPieceMap(m_fallingPieceRot + rotDelta, i);
		m_board[absidxNew] = freeze ? m_fallingPiece : -m_fallingPiece;
	}

	m_fallingPieceRot += rotDelta;
	m_fallingPieceIdx += moveDelta;

	if (freeze)
	{
	    // The held piece becomes available when the current falling piece is locked
		m_alreadyHeld = false;

        // We use > because y is from top down
        if (m_currentHighest > Row(m_fallingPieceIdx))
                m_currentHighest = Row(m_fallingPieceIdx);

		// Lock out
		if (m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, 3) < IdxConvert(0, VANISH_ZONE_HEIGHT))
		{
			m_gameover = true;
		}
	}
}

void Board::UpdateFallingPiece(int rotDelta, int moveDelta)
{
	// Do movement first because we don't want it to stack with wall kicks
	if (ValidMove(0, moveDelta))
		MovePiece(0, moveDelta, false);

	// Loop through the wallkicks at this rotation until one works or they all fail
	int wallKickTable = GetWallKickIdx(m_fallingPieceRot, m_fallingPieceRot + rotDelta);
	int i = 0;
	if (m_fallingPiece == TetrominoData::O || rotDelta == 0) // O pieces should not be rotated / wall kicked at all
		i = 5; // Easy way to fail the condition below and leave it as is
	else if (m_fallingPiece == TetrominoData::I) // The I piece has a different table of wall kicks per SRS
		while (i < 5 && !ValidMove(rotDelta, TetrominoData::iWallKicks[wallKickTable][i]))
			i++;
	else
		while (i < 5 && !ValidMove(rotDelta, TetrominoData::wallKicks[wallKickTable][i]))
			i++;

	if (i != 5)
	{
		int wallKick;
		if (m_fallingPiece == TetrominoData::I)
			wallKick = TetrominoData::iWallKicks[wallKickTable][i];
		else
			wallKick = TetrominoData::wallKicks[wallKickTable][i];
		MovePiece(rotDelta, wallKick, false);
	}
}

int Board::GetWallKickIdx(int startRot, int endRot)
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

int Board::FallingPieceAnchor()
{
	return m_fallingPieceIdx;
}

void Board::Update(Input& input, unsigned int ticks)
{
	if (m_gameover) return;
	m_ticks = ticks;
	if (m_ticks - m_lastTicks >= m_fallRate)
	{
		Fall();
	}

	int moveDelta = 0;
	int rotDelta = 0;

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

	while (m_fallingPieceRot + rotDelta < 0)
		rotDelta += 4;
	while (m_fallingPieceRot + rotDelta > 3)
		rotDelta -= 4;

	UpdateFallingPiece(rotDelta, moveDelta);

}

int Board::IdxConvert(int x, int y)
{
	return y * WIDTH + x;
}

int Board::GetSquare(int x, int y)
{
	return m_board[IdxConvert(x, y)];
}

int Board::TicksPerStep()
{
	return m_fallRate;
}

int Board::Row(int idx)
{
	return idx / WIDTH;
}

int Board::Col(int idx)
{
	return idx % WIDTH;
}

int Board::GetHeldPiece()
{
	return m_heldPiece;
}

int Board::GetHighestPoint()
{
	return m_currentHighest;
}

bool Board::GameOver()
{
	return m_gameover;
}