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
	void Fall();
	void HardDrop();
	void Update(Input& inputs, unsigned int ticks);
	int IdxConvert(int x, int y);
	int GetSquare(int x, int y);
	
	int TicksPerStep();

	int FallingPieceAnchor();

private:
	int GetPieceMap(int rot, int idx);
	bool ValidMove(int rotDelta, int moveDelta);
	void MovePiece(int rotDelta, int moveDelta, bool freeze);
	void UpdateFallingPiece(int rotDelta, int moveDelta);
	int GetWallKickIdx(int startRot, int endRot);
	void ClearLines();

	static int Row(int idx);
	static int Col(int idx);


	bool m_gameover;

	unsigned int m_ticks;
	unsigned int m_lastTicks;
	int m_fallRate;

	int m_board[180];
	int m_fallingPiece;
	int m_fallingPieceRot;
	int m_fallingPieceIdx;
};