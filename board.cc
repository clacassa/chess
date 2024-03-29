/*
 * board.cc
 * This file is part of chess, a console chess engine.
 * Copyright (C) 2023 Cyprien Lacassagne

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <array>
#include "board.h"
#include "view.h"

static Chessboard chessboard = {
    'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R', // 1
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', // 2
    '.', '.', '.', '.', '.', '.', '.', '.', // 3
    '.', '.', '.', '.', '.', '.', '.', '.', // 4
    '.', '.', '.', '.', '.', '.', '.', '.', // 5
    '.', '.', '.', '.', '.', '.', '.', '.', // 6
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p', // 7
    'r', 'n', 'b', 'k', 'q', 'b', 'n', 'r'  // 8
//   h    g    f    e    d    c    b    a 
};

void print_ascii() {
    for (auto rank : chessboard) {
        for (auto sq : rank) {
            std::wcout << sq;
        }
        std::wcout << "\n";
    }
}

void write_piece(char code, char file, int rank) {
    chessboard[rank-1][int('h')-int(file)] = code;
}

void empty_board() {
    for (auto& rank : chessboard) {
        for (auto& sq : rank) {
            if (sq != en_passant_sqr)
                sq = '.';
        }
    }
}

void write_en_passant_sqr(char file, int rank) {
    chessboard[rank-1][int('h')-int(file)] = en_passant_sqr;
}

void clear_en_passant_sqr(char file, int rank) {
    chessboard[rank-1][int('h')-int(file)] = '.';
}

void clear_en_passant_sqr() {
    for (auto& rank : chessboard) {
        for (auto& sq : rank) {
            if (sq == en_passant_sqr)
                sq = '.';
        }
    }
}

bool is_en_passant_sqr(char file, int rank) {
    return (chessboard[rank-1][int('h')-int(file)] == en_passant_sqr);
}

bool is_any_en_psst_sqr() {
    for (auto rank : chessboard) {
        for (auto sq : rank) {
            if (sq == en_passant_sqr)
                return true;
        }
    }
    return false;
}

Square get_en_passant_sqr() {
    for (size_t i(0); i < chessboard.size(); ++i) {
        for (size_t j(0); j < chessboard[i].size(); ++j) {
            if (chessboard[i][j] == en_passant_sqr)
                return {char('h'-j), int(i+1)};
        }
    }
    return {blank, 9};
}

int piece_occurences(char code) {
    int occurences(0);
    for (auto rank : chessboard) {
        for (auto sq : rank) {
            if (sq == code)
                ++occurences;
        }
    }
    return occurences;
}

bool is_friendly(char code, char file, int rank) {
    char p(chessboard[rank-1][int('h')-int(file)]);
    if (code >= 'A' && code <= 'R' && p >= 'A' && p <= 'R') {
        return true;
    }
    if (code >= 'a' && code <= 'r' && p >= 'a' && p <= 'r') {
        return true;
    }
    return false;
}

bool is_enemy(char code, char file, int rank) {
    char p(chessboard[rank-1][int('h')-int(file)]);
    if (code >= 'A' && code <= 'R' && p >= 'a' && p <= 'r') {
        return true;
    }
    if (code >= 'a' && code <= 'r' && p >= 'A' && p <= 'R') {
        return true;
    }
    return false;
}

bool is_enemy(bool w_to_play, char file, int rank) {
    char p(chessboard[rank-1][int('h')-int(file)]);
    if (w_to_play && (p >= 'b' && p <= 'r'))
        return true;
    if (!w_to_play && (p >= 'B' && p <= 'R'))
        return true;
    return false;
}

bool is_empty(char file, int rank) {
    char c(chessboard[rank-1][int('h')-int(file)]);
    return (c == '.' || c == en_passant_sqr);
}

bool is_enemy_king(char code, char file, int rank) {
    char p(chessboard[rank-1][int('h')-int(file)]);
    return (code >= 'A' && code <= 'R' ? p == 'k' : p == 'K');
}

void board::print_board(bool w_pov, Square start_sqr, Square target_sqr, bool check,
                                                                        bool cvc) {
    view::print_board(chessboard, w_pov, start_sqr, target_sqr, check, cvc);
}

void board::print_board(bool w_pov) {
    view::print_board(chessboard, w_pov);
}
