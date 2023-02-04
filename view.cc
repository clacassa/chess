/*
 *  view.cc -- chess -- Chess game in the terminal
 *  Copyright (C) 2023 Cyprien Lacassagne

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include "view.h"

static ColorScheme scheme(beige, brown);

void color_scheme_menu() {
    std::wcout << "You can choose a color scheme among the following: \n";
    for (size_t i(0); i < c_schemes.size(); ++i) {

        std::wstring sgr(c_schemes[i].dark_sqr + L"  " + reset_sgr +
                         c_schemes[i].light_sqr + L"  " + reset_sgr +
                         c_schemes[i].dark_sqr + b_king + reset_sgr +
                         c_schemes[i].light_sqr + w_king + reset_sgr);

        std::wcout << "[" << i << "]\t" << sgr << "\n";
    }
}

void set_color_scheme(int id) {
    scheme = c_schemes[id];
}

void print_board(Board b, bool white_pov, Square start_sqr, Square target_sqr,
                 bool check) {
    if (white_pov) {
        for (int i(board_size-1); i >= 0; --i) {
            std::wcout << i + 1 << " ";
            for (int j(board_size-1); j >= 0; --j) {
                std::wcout << get_graphic_string(white_pov, b[i][j], j, i, 
                                                start_sqr, target_sqr, check);
            }
            std::wcout << reset_sgr << "\n";
        }
        std::wcout << empty_square;
        char file('a');
        for (unsigned i(0); i < board_size; ++i) {
            std::wcout << file << " ";
            ++file;
        }
    }else {
        for (size_t i(0); i < board_size; ++i) {
            std::wcout << i + 1 << " ";
            for (size_t j(0); j < board_size; ++j) {
                std::wcout << get_graphic_string(white_pov, b[i][j], j, i,
                                                start_sqr, target_sqr, check);
            }
            std::wcout << reset_sgr << "\n";
        }
        std::wcout << empty_square;
        char file('h');
        for (unsigned i(0); i < board_size; ++i) {
            std::wcout << file << " ";
            --file;
        }
    }
    std::wcout << "\n";
}

std::wstring get_graphic_string(bool w_turn, char code, int file, int rank,
                                Square start_sqr, Square target_sqr, bool check) {
    std::wstring sgr_square;

    // Chessboard alternating pattern.
    if (rank % 2) {
        if (file % 2) sgr_square = scheme.light_sqr;
        else sgr_square = scheme.dark_sqr;
    }else {
        if (file % 2) sgr_square = scheme.dark_sqr;
        else sgr_square = scheme.light_sqr;
    }

    // Green background for the start and target squares.
    char strt_file(start_sqr.file);
    int strt_rank(start_sqr.rank);
    if (strt_file != blank && strt_rank >= 1 && strt_rank <= board_size) {
        if (rank == strt_rank-1 && file == int('h')-int(strt_file)) {
            if (rank % 2) {
                if (file % 2) sgr_square = light_target_sqr;
                else sgr_square = dark_target_sqr;
            }else {
                if (file % 2) sgr_square = dark_target_sqr;
                else sgr_square = light_target_sqr;
            }
        }
    }
    char trgt_file(target_sqr.file);
    int trgt_rank(target_sqr.rank);
    if (trgt_file != blank && trgt_rank >= 1 && trgt_rank <= board_size) {
        if (rank == trgt_rank-1 && file == int('h')-int(trgt_file)) {
            if (rank % 2) {
                if (file % 2) sgr_square = light_target_sqr;
                else sgr_square = dark_target_sqr;
            }else {
                if (file % 2) sgr_square = dark_target_sqr;
                else sgr_square = light_target_sqr;
            }
        }
    }

    // Red background in case of a check.
    if (check) {
        if (w_turn) {
            if (code == 'K') sgr_square = check_sqr;
        }else if (code == 'k') sgr_square = check_sqr;
    }

    // Add the correct foreground sgr depending on the piece to print.
    code_to_sgr(sgr_square, code);
    
    return sgr_square;
}

void code_to_sgr(std::wstring& sgr_string, char code) {
    switch (code) {
        case '.':
        case '!':
            sgr_string += empty_square;
            break;
        case 'K':
            sgr_string += w_king;
            break;
        case 'Q':
            sgr_string += w_queen;
            break;
        case 'R':
            sgr_string += w_rook;
            break;
        case 'B':
            sgr_string += w_bishop;
            break;
        case 'N':
            sgr_string += w_knight;
            break;
        case 'P':
            sgr_string += w_pawn;
            break;
        case 'k':
            sgr_string += b_king;
            break;
        case 'q':
            sgr_string += b_queen;
            break;
        case 'r':
            sgr_string += b_rook;
            break;
        case 'b':
            sgr_string += b_bishop;
            break;
        case 'n':
            sgr_string += b_knight;
            break;
        case 'p':
            sgr_string += b_pawn;
            break;
        // case '!':
        //     sgr_string += lilas + L"b ";
        //     break;
        default:
            sgr_string += L"\x1b[31mW ";
    }
}
