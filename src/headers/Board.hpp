#pragma once

#include "Tetrominoes.hpp"

class Board
{
private:
	bool m_Gameover;

	int m_Ticks;
	int m_fallRate;

	int m_Board[180] = { 0 };
	int m_FallingPiece;
	int m_FallingPieceRot;
	int m_FallingPieceIdx;
	int* m_pFallingPieceMap;

public:
	static const int WIDTH = 10;
	static const int HEIGHT = 18;

	Board(int fallRate);

	void NewPiece();
	void MoveDown();
	void Update(Uint32 ticks);
	int IdxConvert(int x, int y);
	int GetSquare(int x, int y);
	
	int TicksPerStep();
};