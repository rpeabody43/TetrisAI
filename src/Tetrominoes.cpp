#include "headers/Tetrominoes.hpp"

namespace Tetrominoes
{
	int maps[7][4][4]
	{
		{ // I Piece
			// TODO : I rotates differently than other pieces, add special cases for it
			{0, -2, -1, 1}, {0, -10, 10, -20}, {0, -2, -1, 1}, {0, -10, 10, -20}
		},
		{ // J Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{ // L Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{ // O Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{ // S Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{ // Z Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
		{ // T Piece
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
		},
	};
}