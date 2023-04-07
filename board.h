#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <array>
#include "common.h"

namespace board {
    void print_board(bool w_pov, Square start_sqr, Square target_sqr,
                     bool check, bool cvc);
    void print_board(bool w_pov);
}

constexpr char en_passant_sqr('!');

void print_ascii();

void write_piece(char code, char file, int rank);
void empty_board();
void write_en_passant_sqr(char file, int rank);
void clear_en_passant_sqr(char file, int rank);
void clear_en_passant_sqr();

bool is_en_passant_sqr(char file, int rank);
bool is_any_en_psst_sqr();
Square get_en_passant_sqr();

int piece_occurences(char code);

/**
 * @param code The code of the friendly side piece.
 * @param file The square's file.
 * @param rank The square's rank.
 * @return <tt>true</tt> if the square is occupied by a friendly piece.
 */
bool is_friendly(char code, char file, int rank);

/**
 * @param code The code of the friendly side piece.
 * @param file The square's file.
 * @param rank The square's rank.
 * @return <tt>true</tt> if the square is occupied by an enemy piece.
 */
bool is_enemy(char code, char file, int rank);

/**
 * @param w_to_play If it's White to play.
 * @param file The square's file.
 * @param rank The square's rank.
 * @return <tt>true</tt> if the square is occupied by an enemy piece.
 */
bool is_enemy(bool w_to_play, char file, int rank);
bool is_empty(char file, int rank);
bool is_enemy_king(char code, char file, int rank);

// std::vector<Square> get_friendly_sqrs(char code, char file, int rank);

#endif