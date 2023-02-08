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

namespace {

    struct ColorScheme {
        std::wstring light_sqr;
        std::wstring dark_sqr;
        ColorScheme(std::wstring lt, std::wstring dk)
        :   light_sqr(lt), dark_sqr(dk) {}
    };

    bool alt_piece_style(false);

    /* Special squares */
    const std::wstring empty_square(L"  ");
    const std::wstring light_target_sqr(L"\x1b[48;5;107m");
    const std::wstring dark_target_sqr(L"\x1b[48;5;101m");
    const std::wstring check_sqr(L"\x1b[48;5;168m");

    /* Pieces */
    const std::wstring w_king(L"\x1b[97m\u265A ");
    const std::wstring w_queen(L"\x1b[97m\u265B ");
    const std::wstring w_rook(L"\x1b[97m\u265C ");
    const std::wstring w_bishop(L"\x1b[97m\u265D ");
    const std::wstring w_knight(L"\x1b[97m\u265E ");
    const std::wstring w_pawn(L"\x1b[97m\u2659 ");
    // Atternate style for the white pieces
    const std::wstring w_king_bis(L"\x1b[30m\u2654 ");
    const std::wstring w_queen_bis(L"\x1b[30m\u2655 ");
    const std::wstring w_rook_bis(L"\x1b[30m\u2656 ");
    const std::wstring w_bishop_bis(L"\x1b[30m\u2657 ");
    const std::wstring w_knight_bis(L"\x1b[30m\u2658 ");
    const std::wstring w_pawn_bis(L"\x1b[30m\u2659 ");

    const std::wstring b_king(L"\x1b[30m\u265A ");
    const std::wstring b_queen(L"\x1b[30m\u265B ");
    const std::wstring b_rook(L"\x1b[30m\u265C ");
    const std::wstring b_bishop(L"\x1b[30m\u265D ");
    const std::wstring b_knight(L"\x1b[30m\u265E ");
    const std::wstring b_pawn(L"\x1b[30m\u265F ");

    /* Colors */
    const std::wstring beige(L"\x1b[48;5;216m");
    const std::wstring brown(L"\x1b[48;5;95m");
    const std::wstring navy(L"\x1b[48;5;24m");
    const std::wstring skyblue(L"\x1b[48;5;75m");
    const std::wstring light_grey(L"\x1b[48;5;252m");
    const std::wstring yellowish_brown(L"\x1b[48;5;137m");
    const std::wstring light_beige(L"\x1b[48;5;216m");
    const std::wstring grey(L"\x1b[48;5;248m");
    const std::wstring magenta(L"\x1b[48;5;5m");

    /* Color schemes */
    const ColorScheme standard(beige, brown);
    const ColorScheme gnu_chess(light_grey, magenta);
    const ColorScheme grey_blue(grey, navy);
    const ColorScheme deep_blue(skyblue, navy);

    ColorScheme scheme(beige, brown);
    const std::vector<ColorScheme> c_schemes { standard, gnu_chess, grey_blue, 
                                                                    deep_blue };

    void color_scheme_menu();
    void set_color_scheme(size_t id);
    std::wstring get_graphic_string(bool w_turn, char code, int col, int rank,
                                Square start_sqr, Square target_sqr, bool check);
    void code_to_sgr(std::wstring& sgr_string, char code);
}


bool customize_style() {
    color_scheme_menu();
    int select;
    if (!(std::wcin >> select)) {
        std::wcin.clear();
        std::wcin.ignore(10000, '\n');
        std::wcout << "Expected an integer\n";
        return false;
    }
    set_color_scheme(select);

    std::wcout << "White pieces style options: \n[0]\t"
               << scheme.dark_sqr << w_king << scheme.light_sqr << w_queen
               << scheme.dark_sqr << w_bishop << scheme.light_sqr << w_knight
               << scheme.dark_sqr << w_rook << scheme.light_sqr << w_pawn
               << reset_sgr << "  (default)\n";
    
    std::wcout << "[1]\t"
               << scheme.dark_sqr << w_king_bis << scheme.light_sqr << w_queen_bis
               << scheme.dark_sqr << w_bishop_bis << scheme.light_sqr << w_knight_bis
               << scheme.dark_sqr << w_rook_bis << scheme.light_sqr << w_pawn_bis
               << reset_sgr << "\n";

    if (!(std::wcin >> select)) {
        std::wcin.clear();
        std::wcin.ignore(10000, '\n');
        std::wcout << "Expected an integer\n";
        return false;
    }
    if (select == 1)
        alt_piece_style = true;

    return true;
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


namespace {

    void color_scheme_menu() {
        std::wcout << "You can choose a color scheme among the following: \n";
        for (size_t i(0); i < c_schemes.size(); ++i) {

            std::wstring sgr(c_schemes[i].dark_sqr + L"  " + reset_sgr +
                            c_schemes[i].light_sqr + L"  " + reset_sgr +
                            c_schemes[i].dark_sqr + b_king + reset_sgr +
                            c_schemes[i].light_sqr + w_king + reset_sgr);

            std::wcout << "[" << i << "]\t" << sgr << (i == 0 ? "  (default)\n" : "\n");
        }
    }

    void set_color_scheme(size_t id) {
        if (id >= 0 && id < c_schemes.size())
            scheme = c_schemes[id];
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
            // case '!':
                sgr_string += empty_square;
                break;
            case 'K':
                sgr_string += (alt_piece_style ? w_king_bis : w_king);
                break;
            case 'Q':
                sgr_string += (alt_piece_style ? w_queen_bis : w_queen);
                break;
            case 'R':
                sgr_string += (alt_piece_style ? w_rook_bis : w_rook);
                break;
            case 'B':
                sgr_string += (alt_piece_style ? w_bishop_bis : w_bishop);
                break;
            case 'N':
                sgr_string += (alt_piece_style ? w_knight_bis : w_knight);
                break;
            case 'P':
                sgr_string += (alt_piece_style ? w_pawn_bis : w_pawn);
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
            case '!':
                sgr_string += magenta + L"b ";
                break;
            default:
                sgr_string += L"\x1b[31m? ";
        }
    }

}