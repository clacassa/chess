/*
 * chess.cc
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

int parse_cli_args(int argc, char ** argv, std::wstring prog_name, Game& game, 
                   bool& black, bool& pvp, bool& cvc);
void usage(std::wstring prog_name);
void version(std::wstring prog_name);
bool verify_FEN_file(std::string FEN_filename);
bool load_FEN_file(Game& game, std::string FEN_filename);

bool is_string_an_int(std::string str);

int main(int argc, char ** argv) {
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
#else
    std::setlocale(LC_ALL, "en_US.UTF-8");
#endif

    srand((unsigned) time(0));

    std::wstring prog_name;
    size_t pos(0);
    for (size_t i(0); i < strlen(argv[0]); ++i) {
        if (argv[0][i] == '/' || argv[0][i] == '\\') {
            pos = i;
        }
    }
    for (size_t i(pos+1); i < strlen(argv[0]); ++i) {
        prog_name += argv[0][i];
    }

    Game game;
    bool black(false), pvp(false), cvc(false);
    
    /* Parse CLI args */
    int parse_result(parse_cli_args(argc, argv, prog_name, game, black, pvp, cvc));
    if (parse_result == 1)
        return 1;
    else if (parse_result == 2)
        return 0;

    game.game_flow(black, pvp, cvc);

    return 0;
}

int parse_cli_args(int argc, char ** argv, std::wstring prog_name, Game& game, 
                   bool& black, bool& pvp, bool& cvc) {
    std::string FEN_filename;
    if (argc == 1) {
        usage(prog_name);
        std::wcout << "\n";
    }

    if (argc > 4)
        return 1;
    if (argc >= 2) {
        for (int i(1); i < argc; ++i) {
            if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--perft") == 0) {

                bool depth_arg(false), fen_arg(false);

                if (i+1 < argc) {
                    if (is_string_an_int(argv[i+1]))
                        depth_arg = true;
                    else {
                        FEN_filename = argv[i+1];
                        if (!load_FEN_file(game, FEN_filename))
                            return 1;
                        fen_arg = true;
                    }
                }

                if (i+2 < argc) {
                    FEN_filename = argv[i+2];
                    if (!load_FEN_file(game, FEN_filename))
                        return 1;
                    fen_arg = true;
                }

                if (!fen_arg)
                    game.parse_fen(ini_board);

                game.updt_board();
                std::wcout << "Position: \n";
                game.print_position();
                if (depth_arg)
                    game.test_gen_moves(std::stoi(argv[i+1]));
                else
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
                usage(prog_name);
                return 2;
            }else if (strcmp(argv[i], "--version") == 0) {
                version(prog_name);
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

void usage(std::wstring prog_name) {
    std::wcout << "***CHESS in the terminal***\n"
               << "usage : " << prog_name << " [options] [fen-file]\n"
               << "options :\n"
               << "  -b, --black\t\tPlay as Black.\n"
               << "  --pvp\t\t\tPlay locally against a friend.\n"
               << "  -c, --computer-dual\tWitness the computer playing against himself.\n"
               << "  -t, --perft [depth=5]\tRun a performance test up to " << italic << "depth"
               << reset_sgr << " plies from the spcified FEN,\n"
                  "\t\t\t  or from the default board if no file is given, then exit.\n"
               << "  --style\t\tSelect a color scheme for the chessboard.\n\n"
               << "  --help\t\tPrint this message and exit.\n"
               << "  --version\t\tPrint version information and exit.\n";
}


void version(std::wstring prog_name) {
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
        
        if (!game.parse_fen(FEN_string))
            return false;
    }
    return true;
}

bool is_string_an_int(std::string str) {
    for (size_t i(0); i < str.length(); ++i) {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}
