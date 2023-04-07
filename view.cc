/*
 * view.cc
 * This file is part of chess, a console chess engine
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
#include <string>
#include "view.h"

namespace {
    /* Special squares */
    const std::wstring empty_square(L"  ");
    const std::wstring pAscii_light_sqr(L". ");
    const std::wstring pAscii_dark_sqr(L"# ");
    const std::wstring ascii_square(L". ");
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

    // pseudo_ascii style (uncolored)
    const std::wstring pAscii_w_king(L"\u2654 ");
    const std::wstring pAscii_w_queen(L"\u2655 ");
    const std::wstring pAscii_w_rook(L"\u2656 ");
    const std::wstring pAscii_w_bishop(L"\u2657 ");
    const std::wstring pAscii_w_knight(L"\u2658 ");
    const std::wstring pAscii_w_pawn(L"\u2659 ");

    const std::wstring pAscii_b_king(L"\u265A ");
    const std::wstring pAscii_b_queen(L"\u265B ");
    const std::wstring pAscii_b_rook(L"\u265C ");
    const std::wstring pAscii_b_bishop(L"\u265D ");
    const std::wstring pAscii_b_knight(L"\u265E ");
    const std::wstring pAscii_b_pawn(L"\u265F ");
    
    // ASCII style (alphabetic representation)
    const std::wstring ascii_w_king(L"K ");
    const std::wstring ascii_w_queen(L"Q ");
    const std::wstring ascii_w_rook(L"R ");
    const std::wstring ascii_w_bishop(L"B ");
    const std::wstring ascii_w_knight(L"N ");
    const std::wstring ascii_w_pawn(L"P ");

    const std::wstring ascii_b_king(L"k ");
    const std::wstring ascii_b_queen(L"q ");
    const std::wstring ascii_b_rook(L"r ");
    const std::wstring ascii_b_bishop(L"b ");
    const std::wstring ascii_b_knight(L"n ");
    const std::wstring ascii_b_pawn(L"p ");

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
    const std::wstring green(L"\x1b[48;5;28m");

    struct ColorScheme {
        std::wstring light_sqr;
        std::wstring dark_sqr;
        ColorScheme(std::wstring lt, std::wstring dk)
        :   light_sqr(lt), dark_sqr(dk) {}
    };

    /* Color schemes */
    const ColorScheme standard(beige, brown);
    const ColorScheme gnu_chess(light_grey, magenta);
    const ColorScheme grey_green(grey, green);
    const ColorScheme deep_blue(skyblue, navy);

    const std::vector<ColorScheme> c_schemes {
        standard,
        gnu_chess,
        grey_green, 
        deep_blue
    };

    void color_scheme_menu();
    void set_color_scheme(size_t id);
    std::wstring get_graphic_string(bool w_turn, char code, int col, int rank,
                                    Square start_sqr, Square target_sqr, bool check);
    std::wstring get_graphic_string(bool w_pov, char code, int col, int rank);
    void code_to_sgr(std::wstring& sgr_string, char code);

    bool alt_piece_style(false);
    bool pseudo_ascii(false);
    bool ascii(false);
    std::wstring output_string;
    ColorScheme scheme(beige, brown);
} /* unnamed namespace */


bool customize_style() {
    color_scheme_menu();
    size_t select;
    if (!(std::wcin >> select)) {
        std::wcin.clear();
        std::wcin.ignore(10000, '\n');
        std::wcout << "Expected an integer\n";
        return false;
    }
    set_color_scheme(select);

    if (select >= c_schemes.size())
        return true;

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
    else
        alt_piece_style = false;

    return true;
}

void view::print_board(Chessboard b, bool white_pov, Square start_sqr, Square target_sqr,
                    bool check, bool cvc) {
    output_string.clear();
    if (white_pov || cvc) {
        for (int i(board_size-1); i >= 0; --i) {
            output_string += std::to_wstring(i + 1) + L" ";
            for (int j(board_size-1); j >= 0; --j) {
                output_string += get_graphic_string(white_pov, b[i][j], j, i, 
                                                    start_sqr, target_sqr, check);
            }
            output_string += reset_sgr + L"\n";
        }
        output_string += empty_square;
        wchar_t file('a');
        for (unsigned i(0); i < board_size; ++i) {
            std::wstring s = {file};
            output_string += s + L" ";
            ++file;
        }
    }else {
        for (size_t i(0); i < board_size; ++i) {
            output_string += std::to_wstring(i + 1) + L" ";
            for (size_t j(0); j < board_size; ++j) {
                output_string += get_graphic_string(white_pov, b[i][j], j, i,
                                                start_sqr, target_sqr, check);
            }
            output_string += reset_sgr + L"\n";
        }
        output_string += empty_square;
        wchar_t file('h');
        for (unsigned i(0); i < board_size; ++i) {
            std::wstring s = {file};
            output_string += s + L" ";
            --file;
        }
    }
    output_string += L"\n";
    std::wcout << output_string;
}

void view::print_board(Chessboard chessboard, bool w_pov) {
    output_string.clear();
    if (w_pov) {
        for (int i(board_size-1); i >= 0; --i) {
            for (int j(board_size-1); j >= 0; --j) {
                output_string += get_graphic_string(w_pov, chessboard[i][j], j, i);
            }
            output_string += reset_sgr + L"\n";
        }
        output_string += empty_square;
    }else {
        for (size_t i(0); i < board_size; ++i) {
            for (size_t j(0); j < board_size; ++j) {
                output_string += get_graphic_string(w_pov, chessboard[i][j], j, i);
            }
            output_string += reset_sgr + L"\n";
        }
        output_string += empty_square;
    }
    output_string += L"\n";
    std::wcout << output_string;
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
        std::wstring pAscii_demo(L"##.." + pAscii_b_king + pAscii_w_king);
        std::wstring ascii_demo(L". . " + ascii_b_king + ascii_w_king);
        std::wcout << "[" << c_schemes.size() << "]\t" << pAscii_demo << "\n"
                   << "[" << c_schemes.size() + 1 << "]\t" << ascii_demo << "\n";
    }

    void set_color_scheme(size_t id) {
        if (id < 0 || id >= c_schemes.size() + 2)
            return;
        if (id == c_schemes.size()) {
            pseudo_ascii = true;
            ascii = false;
            return;
        }
        if (id == c_schemes.size() + 1) {
            ascii = true;
            pseudo_ascii = false;
            return;
        }
        scheme = c_schemes[id];
        pseudo_ascii = false;
        ascii = false;
    }

    std::wstring get_graphic_string(bool w_turn, char code, int file, int rank,
                                    Square start_sqr, Square target_sqr, bool check) {
        std::wstring sgr_square;
        // Chessboard alternating pattern.
        if (rank % 2) {
            if (file % 2) {
                if (pseudo_ascii)
                    sgr_square = pAscii_light_sqr;
                else if (!ascii)
                    sgr_square = scheme.light_sqr;
            }
            else {
                if (pseudo_ascii)
                    sgr_square = pAscii_dark_sqr;
                else if (!ascii)
                    sgr_square = scheme.dark_sqr;
            }
        }else {
            if (file % 2) {
                if (pseudo_ascii)
                    sgr_square = pAscii_dark_sqr;
                else if (!ascii)
                    sgr_square = scheme.dark_sqr;
            }
            else {
                if (pseudo_ascii)
                    sgr_square = pAscii_light_sqr;
                else if (!ascii)
                    sgr_square = scheme.light_sqr;
            }
        }

        // Green background for the start and target squares.
        if (!pseudo_ascii && !ascii) {
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

    std::wstring get_graphic_string(bool w_pov, char code, int file, int rank) {
        std::wstring sgr_square;
        // Chessboard alternating pattern.
        if (rank % 2) {
            if (file % 2) {
                if (pseudo_ascii)
                    sgr_square = pAscii_light_sqr;
                else if (!ascii)
                    sgr_square = scheme.light_sqr;
            }
            else {
                if (pseudo_ascii)
                    sgr_square = pAscii_dark_sqr;
                else if (!ascii)
                    sgr_square = scheme.dark_sqr;
            }
        }else {
            if (file % 2) {
                if (pseudo_ascii)
                    sgr_square = pAscii_dark_sqr;
                else if (!ascii)
                    sgr_square = scheme.dark_sqr;
            }
            else {
                if (pseudo_ascii)
                    sgr_square = pAscii_light_sqr;
                else if (!ascii)
                    sgr_square = scheme.light_sqr;
            }
        }
        code_to_sgr(sgr_square, code);
        return sgr_square;
    }

    void code_to_sgr(std::wstring& sgr_string, char code) {
        switch (code) {
            case '.':
            // case '!':
                if (!pseudo_ascii && !ascii)
                    sgr_string += empty_square;
                if (!pseudo_ascii && ascii)
                    sgr_string += ascii_square;
                break;
            case 'K':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_king;
                else if (ascii)
                    sgr_string = ascii_w_king;
                else
                    sgr_string += (alt_piece_style ? w_king_bis : w_king);
                break;
            case 'Q':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_queen;
                else if (ascii)
                    sgr_string = ascii_w_queen;
                else
                    sgr_string += (alt_piece_style ? w_queen_bis : w_queen);
                break;
            case 'R':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_rook;
                else if (ascii)
                    sgr_string = ascii_w_rook;
                else
                    sgr_string += (alt_piece_style ? w_rook_bis : w_rook);
                break;
            case 'B':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_bishop;
                else if (ascii)
                    sgr_string = ascii_w_bishop;
                else
                    sgr_string += (alt_piece_style ? w_bishop_bis : w_bishop);
                break;
            case 'N':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_knight;
                else if (ascii)
                    sgr_string = ascii_w_knight;
                else
                    sgr_string += (alt_piece_style ? w_knight_bis : w_knight);
                break;
            case 'P':
                if (pseudo_ascii)
                    sgr_string = pAscii_w_pawn;
                else if (ascii)
                    sgr_string = ascii_w_pawn;
                else
                    sgr_string += (alt_piece_style ? w_pawn_bis : w_pawn);
                break;
            case 'k':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_king;
                else if (ascii)
                    sgr_string = ascii_b_king;
                else
                    sgr_string += b_king;
                break;
            case 'q':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_queen;
                else if (ascii)
                    sgr_string = ascii_b_queen;
                else
                    sgr_string += b_queen;
                break;
            case 'r':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_rook;
                else if (ascii)
                    sgr_string = ascii_b_rook;
                else
                    sgr_string += b_rook;
                break;
            case 'b':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_bishop;
                else if (ascii)
                    sgr_string = ascii_b_bishop;
                else
                    sgr_string += b_bishop;
                break;
            case 'n':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_knight;
                else if (ascii)
                    sgr_string = ascii_b_knight;
                else
                    sgr_string += b_knight;
                break;
            case 'p':
                if (pseudo_ascii)
                    sgr_string = pAscii_b_pawn;
                else if (ascii)
                    sgr_string = ascii_b_pawn;
                else
                    sgr_string += b_pawn;
                break;
            case '!':
                sgr_string += magenta + L"  " + reset_sgr;
                break;
            default:
                sgr_string += L"\x1b[31m? ";
        }
    }
}
