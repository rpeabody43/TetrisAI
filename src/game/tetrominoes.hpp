#pragma once

#include <cstdint>

#define I_PIECE 1
#define J_PIECE 2
#define L_PIECE 3
#define O_PIECE 4
#define S_PIECE 5
#define Z_PIECE 6
#define T_PIECE 7

namespace TetrominoData
{
    // Piece maps, where to place squares relative to a piece's anchor.
    const uint8_t MAPS[7][4][4] =
        {
            { // I
                {10, 11, 12, 13}, {2, 12, 22, 32}, {20, 21, 22, 23}, {1, 11, 21, 31}
            },
            { // J
                {0,  10, 11, 12}, {1, 2,  11, 21}, {10, 11, 12, 22}, {1, 11, 20, 21}
            },
            { // L
                {2,  10, 11, 12}, {1, 11, 21, 22}, {10, 11, 12, 20}, {0, 1,  11, 21}
            },
            { // O
                {1,  2,  11, 12}, {1, 2,  11, 12}, {1,  2,  11, 12}, {1, 2,  11, 12}
            },
            { // S
                {1,  2,  10, 11}, {1, 11, 12, 22}, {11, 12, 20, 21}, {0, 10, 11, 21}
            },
            { // Z
                {0,  1,  11, 12}, {2, 11, 12, 21}, {10, 11, 21, 22}, {1, 10, 11, 20}
            },
            { // T
                {1,  10, 11, 12}, {1, 11, 12, 21}, {10, 11, 12, 21}, {1, 10, 11, 21}
            },
        };

    /**
     * Get the nth square.
     * @param piece Which piece to get.
     * @param rot What rotation the piece is.
     * @param n The number square to get.
     * @return The square position relative to the anchor.
     */
    inline uint8_t GetPieceMap (uint8_t piece, uint8_t rot, uint8_t n)
    {
        return MAPS[piece - 1][rot][n];
    }

    struct Bounds
    {
        int8_t leftBound;
        int8_t rightBound;
    };
    const Bounds PIECE_BOUNDS[7][4] =
    {
        {{ 0, 6}, {-2, 7}, { 0, 6}, {-1, 8}}, // I
        {{ 0, 7}, {-1, 7}, { 0, 7}, { 0, 8}}, // J
        {{ 0, 7}, {-1, 7}, { 0, 7}, { 0, 8}}, // L
        {{-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}}, // O
        {{ 0, 7}, {-1, 7}, { 0, 7}, { 0, 8}}, // S
        {{ 0, 7}, {-1, 7}, { 0, 7}, { 0, 8}}, // Z
        {{ 0, 7}, {-1, 7}, { 0, 7}, { 0, 8}}  // T
    };

    /**
     * Get the left-most and right-most position a certain piece with certain rotation can be on the board.
     * @param piece Which piece to get.
     * @param rot What rotation the piece is.
     * @return Bounds, a struct with ints leftBound and rightBound
     */
    inline Bounds GetPieceBounds (uint8_t piece, uint8_t rot)
    {
        return PIECE_BOUNDS[piece-1][rot];
    }

    // Piece color hex codes
    const uint32_t HEX_CODES[7] =
    {
        0x00C2DEFF, // I
        0x00418DFF, // J
        0xFA8901FF, // L
        0xFAD717FF, // O
        0x00BA71FF, // S
        0xF43545FF, // T
        0x5F2879FF  // A
    };

    // Wall kicks for pieces that are not I or O
    // https://tetris.fandom.com/wiki/SRS
    const int8_t WALL_KICKS[8][5] =
    {
        {0, -1, -11,  20,  19}, // 0 >> 1
        {0,  1,  11, -20, -19}, // 1 >> 0
        {0,  1,  11, -20, -19}, // 1 >> 2
        {0, -1, -11,  20,  19}, // 2 >> 1
        {0,  1,  -9,  20,  21}, // 2 >> 3
        {0, -1,   9, -20, -21}, // 3 >> 2
        {0, -1,   9, -20, -21}, // 3 >> 0
        {0,  1,  -9,  20,  21}  // 0 >> 3
    };

    // Wall kicks for the I piece
    const int8_t I_WALL_KICKS[8][5] =
    {
        {0, -2,  1,   8, -19},  // 0 >> 1
        {0,  2, -1,  -8,  19},  // 1 >> 0
        {0, -1,  2, -21,  12},  // 1 >> 2
        {0,  1, -2,  21, -12},  // 2 >> 1
        {0,  2, -1,  -8,  19},  // 2 >> 3
        {0, -2,  1,   8, -19},  // 3 >> 2
        {0,  1, -2,  21, -12},  // 3 >> 0
        {0, -1,  2, -21,  12}   // 0 >> 3
    };
}