#include <random>

#include "headers/Tetrominoes.hpp"
#include "headers/Board.hpp"

Board::Board(int ticksPerStep)
{
	mGameover = false;

	mTicks = 0;
	mTicksPerStep = ticksPerStep;
	mFallingPiece = 0;
	mFallingPieceRot = 0;
	mFallingPieceIdx = 5;
	pFallingPieceMap = NULL;
}

void Board::new_piece() 
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(0, 7); // TODO : Proper random

	// mFallingPiece = distr(eng) + 1;
	mFallingPiece = 1;
	mFallingPieceIdx = 5;
	mFallingPieceRot = 0;
	pFallingPieceMap = Tetrominoes::maps[mFallingPiece-1][mFallingPieceRot];

	int start = 5;
	for (int i = 3; i >= 0; i--)
	{
		int newIdx = start + *(pFallingPieceMap + i);
		if (mBoard[newIdx] == 0)
		{
			mBoard[newIdx] = mFallingPiece;
		}
		else mGameover = true;
	}
}

void Board::tick()
{
	mTicks++;
	if (mTicks % mTicksPerStep != 0) return;

	if (mFallingPiece == 0)
	{
		new_piece();
		return;
	}

	bool positionValid = true;
	for (int i = 0; i < 4 && positionValid; i++)
	{
		int newPos = mFallingPieceIdx + *(pFallingPieceMap + i) + 10;
		if (newPos > HEIGHT*WIDTH || mBoard[newPos] != 0)
			positionValid = false;
	}

	if (!positionValid)
	{
		new_piece();
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		// Get the block with the delta from the map array
		int idx = mFallingPieceIdx + *(pFallingPieceMap + i);
		mBoard[idx] = 0;
		mBoard[idx + 10] = mFallingPiece;
	}
	mFallingPieceIdx += 10;
}

int Board::idx_convert(int x, int y)
{
	return y * WIDTH + x;
}

int Board::get_square(int x, int y)
{
	return mBoard[idx_convert(x, y)];
}

int Board::ticks_per_step()
{
	return mTicksPerStep;
}

int Board::total_ticks()
{
	return mTicks;
}