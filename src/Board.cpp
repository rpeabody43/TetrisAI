#include <random>
#include <iostream>

#include "headers/Tetrominoes.hpp"
#include "headers/Board.hpp"


Board::Board(int fallRate)
	: m_gameover(false)
	, m_ticks(0)
	, m_lastTicks(0)
	, m_fallingPiece(0)
	, m_fallingPieceRot(0)
	, m_fallingPieceIdx(3)
	, m_board()
{
	m_fallRate = fallRate;
}

int Board::GetPieceMap(int rot, int idx)
{
	int ret = TetrominoData::maps[m_fallingPiece - 1][rot][idx];
	return ret;
}

void Board::NewPiece()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 6); // TODO : Proper random

	m_fallingPiece = distr(eng) + 1;
	//m_fallingPiece = 1;
	m_fallingPieceIdx = 3; // Point the piece starts drawing from
	m_fallingPieceRot = 0;

	int start = m_fallingPieceIdx;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + GetPieceMap(m_fallingPieceRot, i);
		if (m_board[newIdx] == 0)
		{
			m_board[newIdx] = -m_fallingPiece;
		}
		else m_gameover = true;
	}
}

void Board::MoveDown()
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
		for (int i = 3; i >= 0; i--)
		{
			int idx = m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, i);
			m_board[idx] = m_fallingPiece;
		}
		NewPiece();
		return;
	}

	UpdateFallingPiece(0, 10);
}

bool Board::ValidMove(int rotDelta, int moveDelta)
{
	for (int i = 0; i < 4; i++)
	{
		int oldPos = m_fallingPieceIdx + GetPieceMap(m_fallingPieceRot, i);
		int newPos = m_fallingPieceIdx + moveDelta + GetPieceMap(m_fallingPieceRot+rotDelta, i);
		// If it's an index error on either side
		if (newPos > HEIGHT * WIDTH || newPos < 0)
			return false;
		// If it's a nonempty square, that also isn't the falling piece itself
		if (m_board[newPos] != 0 && m_board[newPos] != -m_fallingPiece)
			return false;
		// If it would hit the side of the board
		bool closeToEdge = ((oldPos % 10 <= 2) && (newPos % 10 >= 8)) || ((oldPos % 10 >= 8) && (newPos % 10 <= 2));
		if (closeToEdge)
			return false;
	}
	return true;
}

// DOESN'T DO ANY CHECKS
// JUST MOVES PIECE AND LEAVES ZEROES IN ITS PLACE
void Board::MovePiece(int rotDelta, int moveDelta)
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
		m_board[absidxNew] = -m_fallingPiece;
	}

	m_fallingPieceRot += rotDelta;
	m_fallingPieceIdx += moveDelta;
}

void Board::UpdateFallingPiece(int rotDelta, int moveDelta)
{
	// Do movement first because we don't want it to stack with wall kicks
	if (ValidMove(0, moveDelta))
		MovePiece(0, moveDelta);

	// Loop through the wallkicks at this rotation until one works or they all fail
	int wallKickTable = GetWallKickIdx(m_fallingPieceRot, m_fallingPieceRot + rotDelta);
	int i = 0;
	if (m_fallingPiece == TetrominoData::o) // O pieces should not be rotated / wall kicked at all
		i = 5; // Easy way to fail the condition below and leave it as is
	else if (m_fallingPiece == TetrominoData::i) // The I piece has a different table of wall kicks per SRS
		while (i < 5 && !ValidMove(rotDelta, TetrominoData::iWallKicks[wallKickTable][i]))
			i++;
	else
		while (i < 5 && !ValidMove(rotDelta, TetrominoData::wallKicks[wallKickTable][i]))
			i++;

	if (i != 5)
	{
		int wallKick;
		if (m_fallingPiece == TetrominoData::i)
			wallKick = TetrominoData::iWallKicks[wallKickTable][i];
		else
			wallKick = TetrominoData::wallKicks[wallKickTable][i];
		MovePiece(rotDelta, wallKick);
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
		MoveDown();
	}

	int moveDelta = 0;
	int rotDelta = 0;

	if (input.moveLeft)
		moveDelta -= 1;
	if (input.moveRight)
		moveDelta += 1;
	if (input.softDrop)
		MoveDown();
	if (input.rotClockwise)
		rotDelta += 1;
	if (input.rotCountClockwise)
		rotDelta -= 1;

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

bool Board::RotRight()
{
	return Rot(true);
}

bool Board::RotLeft()
{
	return Rot(false);
}

bool Board::Rot(bool dir)
{
	int factor = (dir) ? 1 : -1;
	UpdateFallingPiece(factor, 0);
	return true;
}