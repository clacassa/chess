#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include "common.h"

struct ColorScheme {
    std::wstring light_sqr;
    std::wstring dark_sqr;
    ColorScheme(std::wstring lt, std::wstring dk)
        :   light_sqr(lt), dark_sqr(dk) {}
};
using namespace std;

/* SGR for text */
const std::wstring reset_sgr(L"\x1b[0m");
const std::wstring msg_color(L"\x1b[38;5;13m");
const std::wstring clr_screen(L"\x1b[H\x1b[2J");
const std::wstring clr_display(L"\x1b[13F\x1b[J");
const std::wstring erase_line(L"\x1b[100D\x1b[A\x1b[2K");
const std::wstring italic(L"\x1b[3m");

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

const std::vector<ColorScheme> c_schemes { standard, gnu_chess, grey_blue, deep_blue };

/* Functions */
bool customize_style();
void color_scheme_menu();
void set_color_scheme(size_t id);
void print_board(Board, bool white_pov, Square start_sqr, Square target_sqr,
                 bool check);
std::wstring get_graphic_string(bool w_turn, char code, int col, int rank,
                                Square start_sqr, Square target_sqr, bool check);
void code_to_sgr(std::wstring& sgr_string, char code);

#endif
