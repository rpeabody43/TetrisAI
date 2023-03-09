#pragma once

#include "Tetrominoes.hpp"

class Board
{
public:
	static const int WIDTH = 10;
	static const int HEIGHT = 18;

	Board(int fallRate);

	void NewPiece();
	void MoveDown();
	void Update(unsigned int ticks);
	int IdxConvert(int x, int y);
	int GetSquare(int x, int y);
	
	int TicksPerStep();

	bool RotRight();
	bool RotLeft();

	int FallingPieceAnchor();

private:
	void UpdateFallingPiece(int rotDelta, int moveDelta);
	int GetPieceMap(int idx);
	bool Rot(bool dir);

	bool m_gameover;

	unsigned int m_ticks;
	unsigned int m_lastTicks;
	int m_fallRate;

	int m_board[180];
	int m_fallingPiece;
	int m_fallingPieceRot;
	int m_fallingPieceIdx;
};