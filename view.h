#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include "common.h"

/* SGR for text */
const std::wstring reset_sgr(L"\x1b[0m");
const std::wstring msg_color(L"\x1b[38;5;13m");
const std::wstring clr_screen(L"\x1b[H\x1b[2J");
const std::wstring clr_display(L"\x1b[13F\x1b[J");
const std::wstring erase_line(L"\x1b[100D\x1b[A\x1b[2K");
const std::wstring italic(L"\x1b[3m");

/* Functions */
bool customize_style();
void print_board(Board, bool white_pov, Square start_sqr, Square target_sqr, bool check);

#endif
