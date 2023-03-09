#include <random>

#include "headers/Tetrominoes.hpp"
#include "headers/Board.hpp"

Board::Board(int fallRate)
	: m_gameover(false)
	, m_ticks(0)
	, m_lastTicks(0)
	, m_fallingPiece(0)
	, m_fallingPieceRot(0)
	, m_fallingPieceIdx(5)
	, m_pFallingPieceMap(nullptr)
	, m_board()
{
	m_fallRate = fallRate;
}

void Board::NewPiece()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 7); // TODO : Proper random

	// mFallingPiece = distr(eng) + 1;
	m_fallingPiece = 1;
	m_fallingPieceIdx = 5;
	m_fallingPieceRot = 0;
	m_pFallingPieceMap = Tetrominoes::maps[m_fallingPiece-1][m_fallingPieceRot];

	int start = 5;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + *(m_pFallingPieceMap + i);
		if (m_board[newIdx] == 0)
		{
			m_board[newIdx] = m_fallingPiece;
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

	bool positionValid = true;
	for (int i = 0; i < 4 && positionValid; i++)
	{
		int newPos = m_fallingPieceIdx + *(m_pFallingPieceMap + i) + 10;
		if (newPos > HEIGHT * WIDTH || m_board[newPos] != 0)
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
		int idx = m_fallingPieceIdx + *(m_pFallingPieceMap + i);
		m_board[idx] = 0;
		m_board[idx + 10] = m_fallingPiece;
	}
	m_fallingPieceIdx += 10;
}

void Board::Update(unsigned int ticks)
{
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