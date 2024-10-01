#include <cmath>
#include <vector>
#include <cfloat>

#include "eval.hpp"
#include "../../game/Board.hpp"
#include "../../game/tetrominoes.hpp"

struct BoardAnalysis {
    uint8_t holes_count;         // Open squares with filled squares above
    uint16_t aggregate_height;   // The total number of filled squares
    uint8_t complete_lines;      // Amount of lines to be cleared
    double height_std_dev;       // Flatter board = better
    uint8_t highest_point;       // Highest point reached
    uint8_t blocks_over_holes;   // How many blocks are above holes in the board
};

/**
 * @param highest_points An array of the highest points in each column.
 * @return The standard deviation of heights.
 */
double get_height_std_dev (const int highest_points[Board::WIDTH]) {
    int sum = 0, i;
    double avg, dev, standard_dev = 0;
    for (i = 0; i < Board::WIDTH; i++) {
        sum += highest_points[i];
    }

    avg = (double) sum / Board::WIDTH;
    for (i = 0; i < Board::WIDTH; i++) {
        dev = highest_points[i] - avg;
        standard_dev += dev * dev;
    }
    return std::sqrt(standard_dev / Board::WIDTH);
}

/**
 * Runs each of the heuristics on the current board with a hypothetical move.
 * @param current_board The current board state.
 * This method does not modify the Board object.
 * @param piece_anchor Where the proposed move would end.
 * @param piece Which piece the move is with.
 * @param piece_rot The rotation of the piece after the move.
 * @return A BoardAnalysis object with various heuristics
 */
BoardAnalysis analyze_board (
    Board* current_board, int piece_anchor, int piece, int piece_rot
) {
    int piece_squares[4] = {};
    int square_idx = 0;
    for (int i = 0; i < 4; i++) {
        piece_squares[i] = piece_anchor + 
            tetromino_data::get_piece_map(piece, piece_rot, i);
    }

    BoardAnalysis vals = {};
    vals.highest_point = current_board->get_highest_row();
    if (vals.highest_point > Board::row(piece_anchor))
        vals.highest_point = Board::row(piece_anchor);


    int column_heights[Board::WIDTH] = {};
    int column_holes[Board::WIDTH] = {};

    // Fiill all heights with the "lowest" square
    std::fill_n(column_heights, Board::WIDTH, Board::HEIGHT);

    for (int y = vals.highest_point; y < Board::HEIGHT; y++) {
        bool line_complete = true;
        for (int x = 0; x < Board::WIDTH; x++) {
            // Consider the square "filled" if there's something 
            // there or the current piece fills it
            bool piece_at_idx = (
                square_idx < 4 && 
                piece_squares[square_idx] == Board::convert_idx(x, y)
            );

            if (piece_at_idx)
                square_idx++;

            bool square_filled = (
                piece_at_idx ||
                current_board->get_square(x, y) > 0
            );

            // Going from top down, so first filled square is the highest
            if (square_filled && column_heights[x] == Board::HEIGHT)
                column_heights[x] = y;

            if (square_filled) {
                vals.aggregate_height++;
            } else {
                // If this isn't the first square in the column and isn't filled
                if (column_heights[x] < 24) {
                    column_holes[x]++;
                    vals.blocks_over_holes += (y-column_heights[x]) 
                        - column_holes[x];
                }
                line_complete = false;
            }
        }
        if (line_complete)
            vals.complete_lines++;
    }

    for (int holes : column_holes) {
        vals.holes_count += holes;
    }
    vals.height_std_dev = get_height_std_dev(column_heights);
    // Make higher number -> higher on board
    vals.highest_point = Board::HEIGHT-vals.highest_point; 

    return vals;
}

bool valid_start (
    Board* current_board, uint8_t piece, uint16_t anchor, uint8_t rot
) {
    for (int i = 0; i < 4; i++) {
        uint8_t square_index = anchor + 
            tetromino_data::get_piece_map(piece, rot, i);
        if (current_board->get_square(square_index) > 0) {
            return false;
        }
    }
    return true;
}

/**
 * Gets all possible "hard drop" moves on the current board
 * @param current_board The current board state.
 * This method does not modify the Board object.
 * @param current_piece The current falling piece.
 * @param held_piece The held piece or the next piece up if no piece is held.
 * @return A vec of Move objects, each with an ending anchor, 
 * rotation, and if it includes a hold
 */
std::vector<Move> generate_moves (
    Board* current_board, uint8_t current_piece, uint8_t held_piece
) {
    std::vector<Move> move_list;
    for (int8_t piece : {current_piece, held_piece}) {
        uint8_t num_rot;
        switch (piece) {
            case O_PIECE:
                num_rot = 1;
                break;
            case S_PIECE:
            case Z_PIECE:
            case I_PIECE:
                num_rot = 2;
                break;
            default:
                num_rot = 4;
                break;
        }
        for (int rot = 0; rot < num_rot; rot++) {
            tetromino_data::Bounds piece_bounds =
                tetromino_data::get_piece_bounds(piece, rot);
            for (
                uint8_t start_pos = piece_bounds.left_bound + 
                    Board::BUFFER_SQUARES;
                start_pos <= piece_bounds.right_bound + Board::BUFFER_SQUARES;
                start_pos++
            ) {
                if (!valid_start(current_board, piece, start_pos, rot))
                    continue;
                uint16_t ending_pos = current_board->get_ghost(
                    piece, start_pos, rot
                );
                move_list.push_back({
                    .position = ending_pos,
                    .rotation = rot,
                    .hold = piece == held_piece
                });
            }
        }
        if (current_piece == held_piece)
            break;
    }

    return move_list;
}

Move best_move (Board* current_board, Weights& weights) {
    uint8_t current_piece = current_board->get_falling_piece();
    uint8_t held_piece = current_board->get_held_piece();
    // Treat the next piece up as the held piece if nothing is held
    if (held_piece == 0)
        held_piece = current_board->nth_piece(0);

    std::vector<Move> move_list = generate_moves(
        current_board, current_piece, held_piece
    );
    Move best_move = {};
    double best_score = -DBL_MAX;

    for (Move& move : move_list) {
        int piece = move.hold ? held_piece : current_piece;
        BoardAnalysis analysis = analyze_board(
            current_board, move.position, piece, move.rotation
        );

        double score = analysis.holes_count * weights.holes_count +
                       analysis.aggregate_height * weights.aggregate_height +
                       analysis.complete_lines * weights.complete_lines +
                       analysis.height_std_dev * weights.height_std_dev +
                       analysis.highest_point * weights.highest_point +
                       analysis.blocks_over_holes * weights.blocks_over_holes;
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}
