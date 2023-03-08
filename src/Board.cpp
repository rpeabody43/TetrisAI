#include "headers\Board.hpp"
#include <random>

#include "headers/Tetrominoes.hpp"
#include "headers/Board.hpp"

Board::Board(int fallRate)
	: m_Gameover(false)
	, m_Ticks(0)
	, m_FallingPiece(0)
	, m_FallingPieceRot(0)
	, m_FallingPieceIdx(5)
	, m_pFallingPieceMap(nullptr)
	, m_Board(0)
{
	m_fallRate = fallRate;
}

void Board::NewPiece()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 7); // TODO : Proper random

	// mFallingPiece = distr(eng) + 1;
	m_FallingPiece = 1;
	m_FallingPieceIdx = 5;
	m_FallingPieceRot = 0;
	m_pFallingPieceMap = Tetrominoes::maps[m_FallingPiece-1][m_FallingPieceRot];

	int start = 5;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + *(m_pFallingPieceMap + i);
		if (m_Board[newIdx] == 0)
		{
			m_Board[newIdx] = m_FallingPiece;
		}
		else m_Gameover = true;
	}
}

void Board::MoveDown()
{

	if (m_FallingPiece == 0)
	{
		NewPiece();
		return;
	}

	bool positionValid = true;
	for (int i = 0; i < 4 && positionValid; i++)
	{
		int newPos = m_FallingPieceIdx + *(m_pFallingPieceMap + i) + 10;
		if (newPos > HEIGHT * WIDTH || m_Board[newPos] != 0)
			positionValid = false;
	}

	if (!positionValid)
	{
		NewPiece();
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		// Get the block with the delta from the map array
		int idx = m_FallingPieceIdx + *(m_pFallingPieceMap + i);
		m_Board[idx] = 0;
		m_Board[idx + 10] = m_FallingPiece;
	}
	m_FallingPieceIdx += 10;
}

void Board::Update(Uint32 ticks)
{
	m_Ticks = ticks;
	//if (mTicks % mTicksPerStep != 0) return;

}

int Board::IdxConvert(int x, int y)
{
	return y * WIDTH + x;
}

int Board::GetSquare(int x, int y)
{
	return m_Board[IdxConvert(x, y)];
}

int Board::TicksPerStep()
{
	return m_fallRate;
}

int Board::total_ticks()
{
	return m_Ticks;
}