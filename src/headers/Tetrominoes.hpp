#pragma once

namespace TetrominoData 
{
	const int I = 1;
	const int J = 2;
	const int L = 3;
	const int O = 4;
	const int S = 5;
	const int Z = 6;
	const int T = 7;

	extern const int maps[7][4][4];
	extern const unsigned int hexCodes[7];
	extern const int wallKicks[8][5];
	extern const int iWallKicks[8][5];

	int GetPieceMap(int piece, int rot, int idx);
}