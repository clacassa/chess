/*
 * message.cc
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
#include <fstream>
#include <sstream>
#ifdef _WIN32
    #include <Windows.h>
#endif
#include "message.h"
#include "view.h"

void message::open_moves_win() {
    #ifdef _WIN32
        system("start pwsh -nop -nol "
               "-c \"[console]::windowwidth=40; "
                    "[console]::windowheight=10; "
                    "[console]::bufferwidth=[console]::windowwidth; " 
                    "[console]::title='Move History'; "
                    "gc log.txt -Wait\"");
        HWND handle = FindWindow(NULL, "Move History");
        SetWindowPos(handle, NULL, 40, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    #elif __linux__
        system("chmod u+x mv_hist && "
               "gnome-terminal --geometry=60x10+500+400 -- ./mv_hist");
    #endif
}

void message::write_to_history(Move move, bool w_ply, int num, bool cap, bool chk,
                                                           bool chkmt) {
    std::ofstream log;
    log.open(moves_history_file, std::ios_base::app);
    if (w_ply)
        log << num << ". ";
    if (move.piece != 'P' && move.piece != 'p')
        log << move.piece;
    else if (cap)
        log << move.start.file;
    if (cap)
        log << 'x';
    log << move.target.file << move.target.rank;
    if (move.prom != blank)
        log << '=' << move.prom;
    if (chkmt)
        log << '#';
    else if (chk)
        log << '+';
    log << " ";
    if (num % 4 == 0 && !w_ply)
        log << "\n";
}

void message::erase_history_data() {
    std::ofstream log;
    log.open(moves_history_file);
    log << "";
}

void message::illegal_move(std::wstring move) {
    std::wcout << msg_color << "Illegal move : " << reset_sgr << move << "\n";
}

void message::illegal_move(char file, int rank, char piece) {
    std::wcout << msg_color << "Illegal move : " << reset_sgr;
    if (piece != 'P' && piece != 'p')
        std::wcout << piece;
    std::wcout << file << rank << "\n";
}

void message::ambigueous_move(char file, int rank, char piece) {
    std::wcout << msg_color << "Ambigueous move : " << reset_sgr;
    if (piece != 'P' && piece != 'p')
        std::wcout << piece;
    std::wcout << file << rank << "\n";
}

void message::white_resigns() {
    std::wcout << msg_color << "White resigns" << reset_sgr << "\t0-1" << "\n";
}

void message::black_resigns() {
    std::wcout << msg_color << "Black resigns" << reset_sgr << "\t1-0" << "\n";
}

void message::checkmate(bool w_ply) {
    std::wcout << msg_color << "Checkmate" << reset_sgr;
    std::wcout << "\t" << (w_ply ? "1-0" : "0-1") << "\n";
}

void message::stalemate() {
    std::wcout << msg_color << "Stalemate" << reset_sgr << L"\t\u00BD-\u00BD\n";
}

void message::draw() {
    std::wcout << msg_color << "Draw" << reset_sgr << L"\t\t\u00BD-\u00BD\n";
}

void message::capture_error(char file, int rank) {
    std::wcout << msg_color << "There is nothing to capture on " << reset_sgr
               << file << rank << "\n";
}

void message::invalid_san(std::wstring bad_SAN) {
    std::wcout << "Incorrect command\n";
}

void message::fen_parsing_error() {
    std::wcout << "Error: failed parsing the FEN file: wrong format.\n";
}

void message::fen_file_not_found(std::string filename) {
    #ifdef _WIN32
    std::wcout << "Error: failed opening the file.\n";
    #else
    std::cout << "Error: failed opening the file \"" << filename << "\".\n";
    #endif
}

void message::bad_extension(std::string filename) {
    #ifdef _WIN32
    std::wcout << "Error: the specified file is not in FEN format.\n";
    #else
    std::cout << "Error: \"" << filename << "\" is not an FEN file.\n";
    #endif
}
