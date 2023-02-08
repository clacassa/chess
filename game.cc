/*
 *  game.cc -- chess -- Chess game in the terminal
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
#include <array>
#include <cstdlib>
#include "player.h"
#include "piece.h"
#include "board.h"
#include "message.h"
#include "view.h"
#include "common.h"
#include "game.h"

Game::Game()
:   current_move(0, '.', {blank, 0}, {blank, 0}), last_move(current_move),
    SAN_piece(blank), SAN_file(blank), SAN_rank(blank), SAN_spec_file(blank),   
    SAN_spec_rank(blank), SAN_prom_pc(blank), SAN_cap(false), SAN_chk(false),
    SAN_promote(false), w_turn(true), capture(false), q_castle(false), k_castle(false), check(false), checkmate(false), w_en_psst(false), b_en_psst(false), nb_move(1)
{
    message::erase_log_data();
}

Game::~Game() {}

void Game::game_flow(bool as_black, bool pvp, bool cvc) {

    updt_board();
    fen_verify_checks();
    print_position(w_turn);

    std::wstring SAN;
    while (true) {

        if (!pvp && as_black) {
            bool game_over(computer_play());
            if(game_over)
                break;
            updt_board();
            ++nb_move;
        }

        if (!cvc) {
            while (true) {
                reset_san_variables();
                prompt_move();
                std::wcin >> SAN;
                if (parse_cmd(SAN))
                    continue;

                if (is_SAN_valid(SAN)) {
                    // std::wcout << clr_display;
                    Piece* matched_piece(nullptr);
                    if (w_turn)
                        matched_piece = white.unique_piece_for_move(SAN_piece, SAN_file,
                                                                    SAN_rank, SAN_spec_file, SAN_spec_rank);
                    else
                        matched_piece = black.unique_piece_for_move(SAN_piece, SAN_file, 
                                                                    SAN_rank,
                                                                    SAN_spec_file, SAN_spec_rank);
                    if (!matched_piece)
                        continue;

                    if (!is_move_legal(matched_piece, SAN_file, SAN_rank-'0', w_turn)) {
                        message::illegal_move(SAN);
                        continue;
                    }

                    bool end(process_move(matched_piece, SAN_file, SAN_rank-'0', w_turn));
                    if (end)
                        return;
                    else {
                        if (w_turn)
                            ++nb_move;
                        updt_board();
                        w_turn = !w_turn;
                        if (pvp) {
                            print_position(w_turn);
                        }
                        break;
                    }
                }
                else {
                    message::invalid_san(SAN);
                    continue;
                }
            }
        }

        if (!pvp && !as_black) {
            bool game_over(computer_play());
            if(game_over)
                break;
            updt_board();
            print_position(w_turn);
        }

        if (cvc) {
            ++nb_move;
            print_position(w_turn);
        }

        reset_san_variables();
    }
}

void Game::prompt_move() {
    if (w_turn) std::wcout << "\n\x1b[3m" "White to play: " << reset_sgr;
    else std::wcout << "\n\x1b[3m" "Black to play: " << reset_sgr;
}

void Game::print_position(bool w_ply) {
    char p(current_move.piece), f(current_move.target.file);
    int r(current_move.target.rank);
    if (r != 0) {
        std::wcout << "\n  " << nb_move - 1;
        if (w_turn)
            std::wcout << "... ";
        else
            std::wcout << ". ";
        if (p != 'P' && p != 'p')
            std::wcout << p;
        else if (capture)
            std::wcout << current_move.start.file;
        if (capture)
            std::wcout << 'x';
        std::wcout << f << r;
        if (SAN_promote)
            std::wcout << '=' << SAN_prom_pc;
        if (checkmate)
            std::wcout << '#';
        else if (check)
            std::wcout << '+';
        std::wcout << "\n";
    }

    board_print_board(w_ply, start, target, check);
    // print_ascii();
}

void Game::updt_board() {
    empty_board();
    white.write_pieces_on_board();
    black.write_pieces_on_board();
    for (size_t i(0); i < white.get_nb_pieces(); ++i) {
        white.get_piece(i)->updt_cov_sqrs();
    }
    for (size_t i(0); i < black.get_nb_pieces(); ++i) {
        black.get_piece(i)->updt_cov_sqrs();
    }
}

bool Game::parse_fen(std::string fen) {
    char piece(0);
    char file('a');
    int rank(8);
    int count(0);
    int i(0);

    empty_board();
    white.delete_pieces();
    black.delete_pieces();

    size_t c(0);
    while (rank >= 1 && c < fen.length()) {
        count = 1;
        switch (fen[c]) {
            case 'B':
            case 'K':
            case 'N':
            case 'P':
            case 'Q':
            case 'R':
            case 'b':
            case 'k':
            case 'n':
            case 'p':
            case 'q':
            case 'r': piece = fen[c]; break;
            
            case '1' ... '8':
                piece = '.';
                count = fen[c] - '0';
                break;
            case '/':
            case ' ':
                --rank;
                file = 'a';
                ++c;
                continue;
            default:
                message::fen_parsing_error();
                return false;
        }

        piece_from_fen(piece, file, rank);

        for (i = 0; i < count; ++i) {
            ++file;
        }
        ++c;
    }

    w_turn = (fen[c] == 'w');
    c += 2;
    for (i = 0; i < 4; ++i) {
        if (fen[c] == ' ') 
            break;
        ++c;
    }
    return true;
}

void Game::fen_verify_checks() {
    if (w_turn) {
        if (black.attacker()) 
            check = true;
    }else if (white.attacker()) 
        check = true;
}

void Game::piece_from_fen(char code, char file, int rank) {
    switch (code) {
        case 'B':
        case 'K':
        case 'N':
        case 'P':
        case 'Q':
        case 'R':
            white.new_piece(code, file, rank);
            break;
        case 'b':
        case 'k':
        case 'n':
        case 'p':
        case 'q':
        case 'r':
            black.new_piece(code, file, rank);
            break;
        case '.': 
            break;
        default:
            std::wcout << "Couldn't add a piece from the FEN file\n";
            break;
    }
}

bool Game::parse_cmd(std::wstring cmd) {
    if (cmd == L"quit")
        exit(EXIT_SUCCESS);
    if (cmd == L"print") {
        print_position(w_turn);
        return true;
    }
    if (cmd == L"style") {
        customize_style();
        return true;
    }
    if (cmd == L"computer") {
        computer_play();
        updt_board();
        computer_play();
        updt_board();
        print_position(w_turn);
        return true;
    }
    if (cmd == L"moves") {
        message::open_log_win();
        return true;
    }
    if (cmd == L"resign") {
        if (is_resign())
            resign();
        return true;
    }
    return false;
}

bool Game::is_SAN_valid(std::wstring SAN) {
    if (SAN.length() > SAN_max_char || SAN.length() < SAN_min_char)
        return false;

    q_castle = false;
    k_castle = false;
    if (SAN == king_castle) {
        k_castle = true;
        SAN_piece = 'K';
        SAN_file = 'g';
        if(w_turn)
            SAN_rank = '1';
        else
            SAN_rank = '8';
        return true;
    }
    if (SAN == queen_castle) {
        q_castle = true;
        SAN_piece = 'K';
        SAN_file = 'c';
        if (w_turn)
            SAN_rank = '1';
        else
            SAN_rank = '8';
        return true;
    }

    SAN_spec_file = blank;
    SAN_spec_rank = blank;
    SAN_prom_pc = blank;
    SAN_promote = false;
    SAN_chk = false;
    SAN_cap = false;
    bool done(false);
    char c, prev;
    for (size_t i(0); i < SAN.length(); ++i) {
        c = SAN[i];
        if (i > 0) 
            prev = SAN[i-1];
        if (!decode_SAN(c, prev, done, i, SAN.length())) 
            return false;
    }
        
    if (SAN_promote && (w_turn ? SAN_rank != '8' : SAN_rank != '1')) 
        return false;
    
    this->SAN = SAN;

    return true;
}

bool Game::decode_SAN(char c, char prev, bool done, int i, int len) {
    if (done && c != '+' && c != '#' && c != '=') 
        return false;
    switch (c) {
        case 'a' ... 'h':
            if (i == 0) {
                SAN_piece = 'P';
                SAN_file = c;
                break;
            }else if (i == len - 1)
                return false;
            switch (prev) {
                case 'a' ... 'h':
                    SAN_spec_file = prev;
                    SAN_file = c;
                    break;
                case '1' ... '8':
                    SAN_spec_rank = prev;
                    SAN_file = c;
                    break;
                case 'B':
                case 'K':
                case 'N':
                case 'Q':
                case 'R':
                    SAN_file = c;
                    break;       
                case 'x':
                    SAN_file = c;
                    break;
                default:
                    return false;
                    break;             
            }
            break;
        case '1' ... '8':
            switch (prev) {
                case 'a' ... 'h':
                    SAN_rank = c;
                    done = true;
                    break;
                case 'B':
                case 'K':
                case 'N':
                case 'Q':
                case 'R':
                    SAN_spec_rank = c;
                    break;
                default:
                    return false;
                    break;
            }
            break;
        case 'B':
        case 'K':
        case 'N':
        case 'Q':
        case 'R':
            if (i == 0)
                SAN_piece = c;
            else if (i == len-1 && prev == '=')
                SAN_prom_pc = c;
            else 
                return false;
            break;
        case 'x':
            SAN_cap = true;
            if (prev >= 'a' && prev <= 'h')
                SAN_spec_file = prev;
            break;
        case '=':
            SAN_promote = true;
            break;
        case '+':
            if (i != len-1)
                return false;
            SAN_chk = true;
            break;
        case '#':
            if (i != len-1)
                return false;
            break;
        default:
            return false;
            break;
    }
    return true;
}

void Game::reset_san_variables() {
    capture = false;
    SAN_promote = false;
    q_castle = false;
    k_castle = false;
    check = false;
    checkmate = false;
    SAN_chk = false;
    SAN_cap = false;
    SAN_prom_pc = blank;
    SAN_file = blank;
    SAN_rank = blank;
    SAN_spec_file = blank;
    SAN_spec_rank = blank;
}

bool Game::is_move_legal(Piece* p, char trgt_file, int trgt_rank, bool w_ply) {
    
    if (w_ply) {
        if ((k_castle && !white.can_k_castle()) || (q_castle && !white.can_q_castle()))
            return false;
    }else {
        if ((k_castle && !black.can_k_castle()) || (q_castle && !black.can_q_castle()))
            return false;
    }

    // Check for illegal 'king jump'
    if (p->get_code() == 'K' || p->get_code() == 'k') {
        if ((!k_castle && trgt_file == p->get_file() + 2) 
            || (!q_castle && trgt_file == p->get_file() - 2))
            return false;
    }
    
    capture = false;
    if (is_enemy(p->get_code(), trgt_file, trgt_rank)
        || is_en_passant_sqr(trgt_file, trgt_rank))
        capture = true;
    if (handle_check(p, trgt_file, trgt_rank, capture, w_ply))
        return true;
    return false;
}

bool Game::handle_check(Piece* piece, char trgt_file, int trgt_rank, bool capt,bool w_p) {

    Square start_sqr({piece->get_file(), piece->get_rank()});
    // Check that castling doesn't cross a check
    if (w_p && k_castle) {
        for (char f(piece->get_file()); f < 'h'; ++f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (black.attacker())
                return false;
        }
    }else if (w_p && q_castle) {
        for (char f(piece->get_file()); f > 'b'; --f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (black.attacker())
                return false;
        }
    }else if (!w_p && k_castle) {
        for (char f(piece->get_file()); f < 'h'; ++f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (white.attacker())
                return false;
        }
    }else if (!w_p && q_castle) {
        for (char f(piece->get_file()); f > 'b'; --f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (white.attacker())
                return false;
        }
    }

    // Silently execute the move to see if it leads to a rule violation.
    piece->updt_position(trgt_file, trgt_rank,  true);

    bool en_psst(false), illegal(false);
    if (capt) {
        if (is_en_passant_sqr(trgt_file, trgt_rank)) {
            if (w_p) {
                black.hide_piece(trgt_file, trgt_rank-1);
                en_psst = true;
                updt_board();
                if (black.attacker() != nullptr)
                    illegal = true;
                black.reveal_piece(trgt_file, trgt_rank-1);
            }else {
                white.hide_piece(trgt_file, trgt_rank+1);
                en_psst = true;
                updt_board();
                if (white.attacker() != nullptr)
                    illegal = true;
                white.reveal_piece(trgt_file, trgt_rank+1);
            }
        }else {
            if (w_p) {
                black.hide_piece(trgt_file, trgt_rank);
                updt_board();
                if (black.attacker() != nullptr)
                    illegal = true;
                black.reveal_piece(trgt_file, trgt_rank);
            }else {
                white.hide_piece(trgt_file, trgt_rank);
                updt_board();
                if (white.attacker() != nullptr)
                    illegal = true;
                white.reveal_piece(trgt_file, trgt_rank);
            }
        }
    }else {
        updt_board();
        if ((w_p && black.attacker() != nullptr)
            || (!w_p && white.attacker() != nullptr)) {
            illegal = true;
        }
    }
    // Replace the piece on its square.
    piece->updt_position(start_sqr.file, start_sqr.rank, true);
    if (en_psst)
        write_en_passant_sqr(trgt_file, trgt_rank);
    updt_board();
    return !illegal;
}

bool Game::process_move(Piece* p, char trgt_file, int trgt_rank, bool w_ply, bool test) {

    bool e_p(is_en_passant_sqr(trgt_file, trgt_rank));

    last_move = current_move;
    // Save in memory the start and target squares of the active piece
    start = {p->get_file(), p->get_rank()};
    target = {trgt_file, trgt_rank};
    current_move = {nb_move, p->get_code(), start, target};
    if (!w_ply)
        current_move.piece += upcase_shift;

    
    // Update position and in-range squares of the active piece
    p->updt_position(trgt_file, trgt_rank, test);
    p->updt_cov_sqrs();
    updt_board();

    // Castling
    if (w_ply) {
        if (k_castle) {
            white.castle_king_side(test);
        }
        else if (q_castle) {
            white.castle_queen_side(test);
        }
    }else if (k_castle) {
        black.castle_king_side(test);
    }
    else if (q_castle) {
        black.castle_queen_side(test);
    }

    // Detect any en-passant square
    if (white.has_en_passant_sqr())
        w_en_psst = true;
    else if (black.has_en_passant_sqr())
        b_en_psst = true;

    // Delete the captured piece if there is any.
    // Hide it, i.e. make it inactive and unseeable while keeping it in memory
    // when this function is called by 'compute_moves'
    if (capture) {
        ++t_result.n_cptrs;
        if (w_turn) {
            if (e_p) {
                if (test)
                    black.hide_piece(trgt_file, trgt_rank-1);
                else
                    black.piece_captured(trgt_file, trgt_rank-1);
            }else {
                if (test)
                    black.hide_piece(trgt_file, trgt_rank);
                else
                    black.piece_captured(trgt_file, trgt_rank);
            }
        }else {
            if (e_p) {
                if (test)
                    white.hide_piece(trgt_file, trgt_rank+1);
                else
                    white.piece_captured(trgt_file, trgt_rank+1);
            }else {
                if (test)
                    white.hide_piece(trgt_file, trgt_rank);
                else
                    white.piece_captured(trgt_file, trgt_rank);
            }
        }
    }

    if (SAN_promote) {
        if (w_ply) {
            white.piece_captured(trgt_file, trgt_rank);
            white.new_piece(SAN_prom_pc, trgt_file, trgt_rank);
        }else {
            black.piece_captured(trgt_file, trgt_rank);
            black.new_piece(SAN_prom_pc - upcase_shift, trgt_file, trgt_rank);
        }
    }else if (w_ply && p->get_code() == 'P' && trgt_rank == 8) {
        ++t_result.n_proms;
        if (test)
            white.hide_piece(trgt_file, trgt_rank);
        else
            white.piece_captured(trgt_file, trgt_rank);
        white.new_piece('Q', trgt_file, trgt_rank);
        SAN_prom_pc = 'Q';
    }else if (!w_ply && p->get_code() == 'p' && trgt_rank == 1) {
        ++t_result.n_proms;
        if (test)
            black.hide_piece(trgt_file, trgt_rank);
        else
            black.piece_captured(trgt_file, trgt_rank);
        black.new_piece('q', trgt_file, trgt_rank);
        SAN_prom_pc = 'q';
    }

    if (w_ply) {
        if (white.attacker()) {
            attacker = white.attacker();
            check = true;
            ++t_result.n_chks;
        }else {
            attacker = nullptr;
            check = false;
            if (SAN_chk)
                std::wcout << msg_color << "this is not a check\n" << reset_sgr;
        }
    }else {
        if (black.attacker()) {
            attacker = black.attacker();
            check = true;
            ++t_result.n_chks;
        }else {
            attacker = nullptr;
            check = false;
            if (SAN_chk)
                std::wcout << msg_color << "This is not a check\n" << reset_sgr;
        }
    }
    
    if (w_en_psst && !w_ply) {
        clear_en_passant_sqr();
        white.reset_en_passant_sqr();
        w_en_psst = false;
    }else if (b_en_psst && w_ply) {
        clear_en_passant_sqr();
        black.reset_en_passant_sqr();
        b_en_psst = false;
    }
    
    if (check) {
        if (is_checkmate(w_ply)) {
            checkmate = true;
            if (!test) {
                message::write_to_log(current_move, w_ply, capture, check, checkmate,
                                                                        SAN_prom_pc);
                message::checkmate(w_ply);
                updt_board();
                print_position(!w_ply);
            }
            ++t_result.n_chkmates;
            return true;
        }
    }else if (is_draw(w_ply)) {
        updt_board();
        if (!test)
            print_position(!w_ply);
        return true;
    }

    if (!test)
        message::write_to_log(current_move, w_ply, capture, check, checkmate, 
                                                                   SAN_prom_pc);

    // if (test && !w_ply)
    //     print_position(true);

    return false;
}

bool Game::is_checkmate(bool w_ply) {
    Piece* piece(nullptr);
    bool cap(false);
    if (w_ply) {
        size_t nb(black.get_nb_pieces());
        for (size_t i(0); i < nb; ++i) {
            piece = black.get_piece(i);
            for (auto sq : piece->get_cov_sqrs()) {
                cap = false;
                if (is_enemy(piece->get_code(), sq.file, sq.rank))
                    cap = true;
                if (handle_check(piece, sq.file, sq.rank, cap, !w_ply))
                    return false;
            } 
        }
    }else {
        size_t nb(white.get_nb_pieces());
        for (size_t i(0); i < nb; ++i) {
            piece = white.get_piece(i);
            for (auto sq : piece->get_cov_sqrs()) {
                cap = false;
                if (is_enemy(piece->get_code(), sq.file, sq.rank))
                    cap = true;
                if (handle_check(piece, sq.file, sq.rank, cap, !w_ply))
                    return false;
            }
        }
    }
    return true;
}

bool Game::is_draw(bool w_ply) {
    // Draw if only the 2 kings remain on the board.
    if (white.king_is_last() && black.king_is_last()) {
        message::draw();
        return true;
    }
    // Stalemate
    if (is_checkmate(w_ply)) {
        message::stalemate();
        return true;
    }
    return false;
}

int Game::is_resign() {
    const std::wstring prompt = erase_line + italic +
                   L"Do you really want to resign ? [y/n] " + reset_sgr;
    std::wstring response;
    do {
        std::wcout << prompt;
        std::wcin >> response;
    } while (response != L"y" && response != L"n");

    if (response == L"y")
        return 1;
    return 0;
}

void Game::resign() {
    if (w_turn)
        message::white_resigns();
    else
        message::black_resigns();
    exit(0);
}

bool Game::computer_play() {
    reset_san_variables();
    Piece* p(nullptr);
    if (w_turn) {
        size_t n(white.get_nb_pieces());
        while (true) {
            do {
                size_t i(rand() % n);
                p = white.get_piece(i);
                p->updt_cov_sqrs();
            } while (p->get_hidden() || p->get_cov_sqrs().size() == 0);
            
            size_t nsq(p->get_cov_sqrs().size());
            size_t i(rand() % nsq);
            char tf(p->get_cov_sqrs()[i].file);
            int tr(p->get_cov_sqrs()[i].rank);

            if (is_move_legal(p, tf, tr, w_turn)) {
                if (process_move(p, tf, tr, w_turn))
                    return true;
                break;
            }
        }
    }else {
        size_t n(black.get_nb_pieces());
        while (true) {

            char tf;
            int tr;
            if (black.can_k_castle() || black.can_q_castle()) {
                size_t i(0);
                do {
                    p = black.get_piece(i);
                    p->updt_cov_sqrs();
                    ++i;
                } while (p->get_code() != 'k');
                if (black.can_k_castle()) {
                    tf = p->get_file() + 2;
                    tr = p->get_rank();
                    k_castle = true;
                }else {
                    tf = p->get_file() - 2;
                    tr = p->get_rank();
                    q_castle = true;
                }                
            }else {
                do {
                    size_t i(rand() % n);
                    p = black.get_piece(i);
                    p->updt_cov_sqrs();
                } while (p->get_hidden() || p->get_cov_sqrs().size() == 0);

                size_t nsq(p->get_cov_sqrs().size());
                size_t i(rand() % nsq);
                tf = p->get_cov_sqrs()[i].file;
                tr = p->get_cov_sqrs()[i].rank;
            }

            if (is_move_legal(p, tf, tr, w_turn)) {
                if (process_move(p, tf, tr, w_turn))
                    return true;
                break;
            }
        }
    }
    w_turn = !w_turn;
    return false;
}

void Game::test_gen_moves(int max_depth) {
    for (int i(1); i <= max_depth; ++i) {
        t_result = {0, 0, 0, 0, 0};
        std::wcout << "depth: " << i
                   << "\tnodes: " << msg_color << compute_moves(i, true) << reset_sgr
                   << "\tcaptures: " << msg_color << t_result.n_cptrs << reset_sgr
                   << "\tcastles: " << msg_color << t_result.n_cstls << reset_sgr
                   << "\tpromotions: " << msg_color << t_result.n_proms << reset_sgr
                   << "\tchecks: " << msg_color << t_result.n_chks << reset_sgr
                   << "\tcheckmates: " << msg_color << t_result.n_chkmates << reset_sgr << "\n";
    }
}

int Game::compute_moves(int depth, bool w_ply) {
    if (depth == 0) {
        // std::wcout << "+1\n";
        return 1;
    }
    
    reset_san_variables();
    int n_positions(0);
    Piece* p(nullptr);
    if (w_ply) {
        size_t n(white.get_nb_pieces());
        for (size_t i(0); i < n; ++i) {
            if (depth == 5)
                std::wcout << msg_color << i+1 << "/" << n << "\n";
            p = white.get_piece(i);
            if (p->get_hidden())
                continue;
            
            for (auto& sq : p->get_cov_sqrs()) {
                
                k_castle = false;
                q_castle = false;
                bool k_cstl(false), q_cstl(false);
                if (p->get_code() == 'K' && sq.file == p->get_file() + 2
                    && sq.rank == p->get_rank()) {
                    k_castle = true;
                    k_cstl = true;
                }else if (p->get_code() == 'K' && sq.file == p->get_file() - 2
                    && sq.rank == p->get_rank()) {
                    q_castle = true;
                    q_cstl = true;
                }

                if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                    char sf(p->get_file());
                    int sr(p->get_rank());

                    process_move(p, sq.file, sq.rank, w_ply, true);
                    n_positions += compute_moves(depth - 1, !w_ply);

                    if (k_cstl)
                        white.undo_k_castle(true);
                    else if (q_cstl)
                        white.undo_q_castle(true);
                    
                    if (k_cstl || q_cstl)
                        ++t_result.n_cstls;

                    p->updt_position(sf, sr, true);
                    p->reveal();
                    for (size_t i(0); i < black.get_nb_pieces(); ++i) {
                        black.get_piece(i)->reveal();
                    }
                    updt_board();
                }
            }
        }
    }else {
        size_t n(black.get_nb_pieces());
        for (size_t i(0); i < n; ++i) {
            p = black.get_piece(i);
            if (p->get_hidden())
                continue;

            for (auto& sq : p->get_cov_sqrs()) {

                k_castle = false;
                q_castle = false;
                bool k_cstl(false), q_cstl(false);
                if (p->get_code() == 'k' && sq.file == p->get_file() + 2
                    && sq.rank == p->get_rank()) {
                    k_castle = true;
                    k_cstl = true;
                }else if (p->get_code() == 'k' && sq.file == p->get_file() - 2
                    && sq.rank == p->get_rank()) {
                    q_castle = true;
                    q_cstl = true;
                }

                if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                    char sf(p->get_file());
                    int sr(p->get_rank());

                    process_move(p, sq.file, sq.rank, w_ply, true);
                    n_positions += compute_moves(depth - 1, !w_ply);

                    if (k_cstl)
                        black.undo_k_castle(true);
                    else if (q_cstl)
                        black.undo_q_castle(true);

                    if (k_cstl || q_cstl)
                        ++t_result.n_cstls;

                    p->updt_position(sf, sr, true);
                    p->reveal();
                    for (size_t i(0); i < white.get_nb_pieces(); ++i) {
                        white.get_piece(i)->reveal();
                    }
                    updt_board();
                }
            }
        }
    }
    return n_positions;
}
