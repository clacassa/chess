/*
 *  chess.cc -- chess -- Chess game in the terminal
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
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <locale>
#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
#endif
#include "message.h"
#include "view.h"
#include "game.h"

int parse_cli_args(int argc, char ** argv, Game& game, bool& black, bool& pvp, bool& cvc);
void usage(char * prog_name);
void version(char * prog_name);
bool verify_FEN_file(std::string FEN_filename);
bool load_FEN_file(Game& game, std::string FEN_filename);

int main(int argc, char ** argv) {
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
    #else
        std::setlocale(LC_ALL, "en_US.UTF-8");
    #endif

    srand((unsigned) time(0));

    Game game;
    bool black(false), pvp(false), cvc(false);
    
    /* Parse CLI args */
    int parse_result(parse_cli_args(argc, argv, game, black, pvp, cvc));
    if (parse_result == 1)
        return 1;
    else if (parse_result == 2)
        return 0;

    game.game_flow(black, pvp, cvc);

    return 0;
}

int parse_cli_args(int argc, char ** argv, Game& game, bool& black, bool& pvp,bool& cvc) {
    std::string FEN_filename;
    if (argc == 1) {
        usage(argv[0]);
        std::wcout << "\n";
    }
    if (argc > 4)
        return 1;
    if (argc >= 2) {
        for (int i(1); i < argc; ++i) {
            if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
                if (i+1 < argc) {
                    FEN_filename = argv[i+1];
                    if (!load_FEN_file(game, FEN_filename))
                        return 1;
                }else
                    game.parse_fen(ini_board);
                game.updt_board();
                game.test_gen_moves();
                return 2;
            }
            else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--black") == 0) {
                black = true;
                game.parse_fen(ini_board);
            }
            else if (strcmp(argv[i], "--pvp") == 0) {
                pvp = true;
                game.parse_fen(ini_board);
            }
            else if (strcmp(argv[i], "-c") == 0
                || strcmp(argv[i], "--computer-dual") == 0) {
                cvc = true;
                game.parse_fen(ini_board);
            }
            else if (strcmp(argv[i], "--style") == 0) {
                if (!customize_style())
                    return 1;
                game.parse_fen(ini_board);
            }
            else if (strcmp(argv[i], "--help") == 0) {
                usage(argv[0]);
                return 2;
            }else if (strcmp(argv[i], "--version") == 0) {
                version(argv[0]);
                return 2;
            }else {
                FEN_filename = argv[i];
                if (!load_FEN_file(game, FEN_filename))
                    return 1;
            }            
        }
    }else
        game.parse_fen(ini_board);
    return 0;
}

void usage(char * prog_name) {
    std::wcout << "***CHESS in the terminal***\n"
               << "usage: " << prog_name << " [options] [fen-file]\n"
               << "options:\n"
               << "  -t, --test\t\tPrint the number of positions up to 6 plies from\n"
                  "\t\t\t  the specified FEN, or from the default board if no file\n"
                  "\t\t\t  is given, and then exit.\n"
               << "  -b, --black\t\tTo play as Black.\n"
               << "  --pvp\t\t\tTo play locally against a friend.\n"
               << "  -c, --computer-dual\tWitness the computer playing against himself.\n"
               << "  --style\t\tTo select a color scheme for the chessboard.\n"
               << "  --help\t\tPrint this message and exit.\n"
               << "  --version\t\tPrint version information and exit.\n";
}

void version(char * prog_name) {
    std::wcout << msg_color << prog_name << " 0.1" << reset_sgr << "\n"
               << "Copyright (c) 2023 Cyprien Lacassagne\n"
               << "This is free software: you are free to change and redistribute it.\n"
               << "There is NO WARRANTY, to the extent perimtted by law.\n";
}

bool verify_FEN_file(std::string FEN_filename) {

    std::ifstream FEN_file(FEN_filename);
    if (FEN_file.fail()) {
        message::fen_file_not_found(FEN_filename);
        return false;
    }

    size_t pos(FEN_filename.find_last_of("."));
    if (FEN_filename.substr(pos + 1) != "fen") {
        message::bad_extension(FEN_filename);
        return false;
    }
    return true;
}

bool load_FEN_file(Game& game, std::string FEN_filename) {
    if (!verify_FEN_file(FEN_filename))
        return false;
    else {
        std::ifstream FEN_file(FEN_filename);
        std::string FEN_string;
        std::ostringstream ss;
        ss << FEN_file.rdbuf();
        FEN_string = ss.str();
        game.parse_fen(FEN_string);
    }
    return true;
}
