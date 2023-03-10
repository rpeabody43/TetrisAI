#pragma once

#include "Tetrominoes.hpp"

struct Input {
	bool moveLeft;
	bool moveRight;
	bool rotClockwise;
	bool rotCountClockwise;
	bool softDrop;
	bool hardDrop;
	bool holdPiece;
};

class Board
{
public:
	static const int WIDTH = 10;
	static const int HEIGHT = 18;

	Board(int fallRate);

	void NewPiece();
	void MoveDown();
	void Update(Input& inputs, unsigned int ticks);
	int IdxConvert(int x, int y);
	int GetSquare(int x, int y);
	
	int TicksPerStep();

	bool RotRight();
	bool RotLeft();

	int FallingPieceAnchor();

private:
	int GetPieceMap(int rot, int idx);
	bool ValidMove(int rotDelta, int moveDelta);
	void MovePiece(int rotDelta, int moveDelta);
	void UpdateFallingPiece(int rotDelta, int moveDelta);
	int GetWallKickIdx(int startRot, int endRot);
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