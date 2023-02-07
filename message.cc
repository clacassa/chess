#include <iostream>
#include <fstream>
#include <sstream>
#ifdef _WIN32
    #include <Windows.h>
#endif
#include "message.h"
#include "view.h"

void message::open_log_win() {
    #ifdef _WIN32
        system("start pwsh -nop -nol -c \"[console]::windowwidth=50; "
               "[console]::windowheight=10; "
               "[console]::bufferwidth=[console]::windowwidth; " 
               "[console]::title='Moves History'; "
               "gc log.txt -Wait -Tail 30\"");
        HWND handle = FindWindow(NULL, "Moves History");
        SetWindowPos(handle, NULL, 40, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    #elif __linux__
        system("gnome-terminal --geometry=50x10+500+400 -- tail -f log.txt ");
    #endif
}

void message::write_to_log(Move move, bool w_ply, bool chk, bool chkmt, char prom_pc) {
    std::ofstream log;
    log.open(log_file, std::ios_base::app);
    if (w_ply)
        log << move.num << ". ";
    if (move.piece != 'P' && move.piece != 'p')
        log << move.piece;
    log << move.sqr.file << move.sqr.rank;
    if (prom_pc != blank)
        log << '=' << prom_pc;
    if (chkmt)
        log << '#';
    else if (chk)
        log << '+';
    log << " ";
    if (move.num % 5 == 0 && !w_ply)
        log << "\n";
}

void message::erase_log_data() {
    std::ofstream log;
    log.open(log_file);
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
    std::wcout << "Incorrect SAN\n";
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
