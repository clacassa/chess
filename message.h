#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include "common.h"

namespace message {

    const std::string log_file("log.txt");
    void open_log_win();
    void write_to_log(Move move, bool w_ply, bool chk, bool chkmt, char prom_pc);
    void erase_log_data();

    // Game flow messages
    void illegal_move(std::wstring move);
    void illegal_move(char file, int rank, char piece);

    void ambigueous_move(char file, int rank, char piece);

    void white_resigns();
    void black_resigns();

    void checkmate(bool w_ply);
    void stalemate();
    void draw();

    void capture_error(char file, int rank);

    // Incorrect Standard Algebraic Notation (SAN)
    void invalid_san(std::wstring bad_SAN);

    // FEN file errors
    void fen_parsing_error();
    void fen_file_not_found(std::string filename);
    void bad_extension(std::string filename);
}

#endif