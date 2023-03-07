#pragma once

public class Board
{
	public:
		static enum Pieces 
		{ 
			I = 1,
			J = 2,
			L = 3,
			O = 4,
			S = 5,
			Z = 6,
			T = 7
		};

		static final int WIDTH = 10;
		static final int HEIGHT = 18;

		bool gameover;

		int ticks;
		int ticksPerStep;

		Board(int ticksPerStep);

		void newPiece();
		void tick();
		int idxConvert(int x int y);
		int getSquare(int x, int y);

	private:
		final int pieceMaps[][][];
		
		int board[];
		int fallingPiece;
		int fallingPieceIdx;
		int* fallingPieceMap;


}