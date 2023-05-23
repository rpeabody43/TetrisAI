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

	void Update(Input& inputs, unsigned int ticks);

	int IdxConvert(int x, int y);
	int GetSquare(int x, int y);

	int GetPieceMap(int rot, int idx);
	int GetPieceMap(int piece, int rot, int idx);

	int NthPiece(int delta);
	
	int TicksPerStep();

	int FallingPieceAnchor();
	int GetHeldPiece();

	bool GameOver();

	static int Row(int idx);
	static int Col(int idx);

private:
	void HoldPiece();
	void NextPiece();
	void NewPiece();
	void NewPiece(int piece);
	void Fall();
	void HardDrop();


	bool ValidMove(int rotDelta, int moveDelta);
	void MovePiece(int rotDelta, int moveDelta, bool freeze);
	void UpdateFallingPiece(int rotDelta, int moveDelta);

	int GetWallKickIdx(int startRot, int endRot);
	void ClearLines();



	bool m_gameover;

	unsigned int m_ticks;
	unsigned int m_lastTicks;
	int m_fallRate;

	int m_board[180];

	int m_fallingPiece;
	int m_fallingPieceRot;
	int m_fallingPieceIdx;

	int m_bags[2][7];
	int m_bagIdx;

	int m_heldPiece;
	bool m_alreadyHeld;
};