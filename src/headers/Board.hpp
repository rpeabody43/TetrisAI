#pragma once

#include "Tetrominoes.hpp"

class Board
{
private:
	bool mGameover;

	int mTicks;
	int mTicksPerStep;

	int mBoard[180] = { 0 };
	int mFallingPiece;
	int mFallingPieceRot;
	int mFallingPieceIdx;
	int* pFallingPieceMap;

public:

	static const int WIDTH = 10;
	static const int HEIGHT = 18;


	Board(int ticksPerStep);

	void new_piece();
	void tick();
	int idx_convert(int x, int y);
	int get_square(int x, int y);
	
	int ticks_per_step();
	int total_ticks();



};