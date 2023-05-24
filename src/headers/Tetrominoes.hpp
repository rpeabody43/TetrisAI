#pragma once

namespace TetrominoData 
{
	const int i = 1;
	const int j = 2;
	const int l = 3;
	const int o = 4;
	const int s = 5;
	const int z = 6;
	const int t = 7;

	extern const int maps[7][4][4];
	extern const unsigned int hexCodes[7];
	extern const int wallKicks[8][5];
	extern const int iWallKicks[8][5];

	int GetPieceMap(int piece, int rot, int idx);
}