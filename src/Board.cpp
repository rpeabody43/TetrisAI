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

int Board::GetPieceMap(int idx)
{
	return Tetrominoes::maps[m_fallingPiece - 1][m_fallingPieceRot][idx];
}

void Board::NewPiece()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 7); // TODO : Proper random

	//m_fallingPiece = distr(eng) + 1;
	m_fallingPiece = 5;
	m_fallingPieceIdx = 23; // Point the piece starts drawing from
	m_fallingPieceRot = 0;

	int start = m_fallingPieceIdx;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + GetPieceMap(i);
		if (m_board[newIdx] == 0)
		{
			m_board[newIdx] = -m_fallingPiece;
		}
		else m_gameover = true;
	}
}

void Board::MoveDown()
{
	if (m_fallingPiece == 0)
	{
		NewPiece();
		return;
	}

	RotRight();

	std::cout << m_fallingPieceRot << std::endl;

	bool positionValid = true;
	for (int i = 0; i < 4 && positionValid; i++)
	{
		int newPos = m_fallingPieceIdx + GetPieceMap(i) + 10;
		if (newPos > HEIGHT * WIDTH || (m_board[newPos] != 0 && m_board[newPos] != -m_fallingPiece))
			positionValid = false;
	}

	// stop piece moving
	if (!positionValid)
	{
		for (int i = 3; i >= 0; i--)
		{
			int idx = m_fallingPieceIdx + GetPieceMap(i);
			m_board[idx] = m_fallingPiece;
		}
		NewPiece();
		return;
	}

	// Move each block forward 10 units, or down one row
	UpdateFallingPiece(0, 10);
}

void Board::UpdateFallingPiece(int rotDelta, int moveDelta)
{
	for (int i = 3; i >= 0; i--)
	{
		// Get the block with the delta from the map array
		int absidxOld = m_fallingPieceIdx + GetPieceMap(i);
		m_board[absidxOld] = 0;
	}
	if (m_fallingPieceRot < 3)
		m_fallingPieceRot += rotDelta;
	else
		m_fallingPieceRot = 0;
	for (int i = 3; i >= 0; i--)
	{
		int absidxNew = m_fallingPieceIdx + moveDelta + GetPieceMap(i);
		m_board[absidxNew] = -m_fallingPiece;
	}

	m_fallingPieceIdx += moveDelta;
}

int Board::FallingPieceAnchor()
{
	return m_fallingPieceIdx;
}

void Board::Update(unsigned int ticks)
{
	if (m_gameover) return;
	m_ticks = ticks;
	if (m_ticks - m_lastTicks >= m_fallRate)
	{
		m_lastTicks = m_ticks;
		MoveDown();
	}

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