#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include "common.h"

/* CSI SEQUENCES */
// SGR for text
const std::wstring reset_sgr(L"\x1b[0m");
const std::wstring msg_color(L"\x1b[38;5;13m");
const std::wstring italic(L"\x1b[3m");
// Erase in display and in line
const std::wstring clr_screen(L"\x1b[H\x1b[2J");
const std::wstring clr_display(L"\x1b[13F\x1b[100D\x1b[J");
const std::wstring clr_end_line(L"\x1b[K");
const std::wstring clr_begin_line(L"\x1b[1K");
const std::wstring clr_line(L"\x1b[2K\x1b");
// Cursor position
const std::wstring cursor_up(L"\x1b[A");
const std::wstring save_cursor_pos(L"\x1b[s");
const std::wstring restore_cursor_pos(L"\x1b[u");

/* Functions */
bool customize_style();
void print_board(Board, bool white_pov, Square start_sqr, Square target_sqr, bool check);

#endif
