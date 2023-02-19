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
// #include <algorithm>
#include "player.h"
#include "piece.h"
#include "board.h"
#include "message.h"
#include "view.h"
#include "common.h"
#include "game.h"

Game::Game()
:   current_move('.', {blank, 0}, {blank, 0}, blank), last_move(current_move),
    SAN_piece(blank), SAN_file(blank), SAN_rank(blank), SAN_spec_file(blank),   
    SAN_spec_rank(blank), SAN_prom_pc(blank), SAN_cap(false), SAN_chk(false),
    SAN_promote(false), w_turn(true), capture(false), q_castle(false), k_castle(false), check(false), checkmate(false), nb_move(1)
{
    message::erase_log_data();
}

Game::~Game() {}

void Game::game_flow(bool as_black, bool pvp, bool cvc) {

    updt_board();
    fen_verify_checks();
    print_position(w_turn, cvc);

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

                    bool end(process_move(matched_piece, SAN_file, SAN_rank-'0',
                                          w_turn, SAN_prom_pc));
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
            if (!cvc)
                print_position(w_turn);
        }

        if (cvc) {
            ++nb_move;
            print_position(w_turn, true);
        }

        reset_san_variables();
    }
}

void Game::prompt_move() {
    if (w_turn) std::wcout << "\n\x1b[3m" "White to play: " << reset_sgr;
    else std::wcout << "\n\x1b[3m" "Black to play: " << reset_sgr;
}

void Game::print_position(bool w_ply, bool cvc) {
    char p(current_move.piece), f(current_move.target.file);
    int r(current_move.target.rank);
    if (cvc)
        std::wcout << clr_display;
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
    white.track_pieces();
    black.track_pieces();
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
        if (fen[c] == '-') {
            ++c; 
            break;
        }
        if (fen[c] == ' ')
            break;
        ++c;
    }

    ++c;
    char ep_file(blank);
    int ep_rank(9);
    for (i = 0; i < 2; ++i) {
        if (fen[c] == '-')
            break;
        if (i == 0)
            ep_file = fen[c];
        else if (i == 1)
            ep_rank = fen[c] - '0';
        ++c;
    }
    if (ep_file != blank) {
        if (ep_rank != 9)
            write_en_passant_sqr(ep_file, ep_rank);
        else {
            message::fen_parsing_error();
            return false;
        }
    }

    white.track_pieces();
    black.track_pieces();

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
    if (cmd == L"gen") {
        for (auto& move : generate_legal_moves(w_turn)) {
            std::wcout << move.start.file << move.start.rank
                       << move.target.file << move.target.rank;
            if (move.prom != blank)
                std::wcout << move.prom;
            std::wcout << "\n";
        }
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

void Game::make_move(Piece* p, Move move, bool w_ply, bool& k_cstl, bool& q_cstl) {

    if ((w_ply && p->get_code() == 'K') || (!w_ply && p->get_code() == 'k')) {
        if (move.target.file == p->get_file() + 2)
            k_cstl = true;
        else if (move.target.file == p->get_file() - 2)
            q_cstl = true;
    }

    k_castle = k_cstl;
    q_castle = q_cstl;

    if (k_cstl || q_cstl)
        ++t_result.n_cstls;

    process_move(p, move.target.file, move.target.rank, w_ply, move.prom, true);
}

void Game::unmake_move(Piece* p, Move move, bool w_ply, bool k_cstl, bool q_cstl) {
    if (k_cstl)
        (w_ply ? white.undo_k_castle(true) : black.undo_k_castle(true));
    else if (q_cstl)
        (w_ply ? white.undo_q_castle(true) : black.undo_q_castle(true));

    if (w_ply && is_en_passant_sqr(move.start.file, move.start.rank + 1)) {
        // white.reset_en_passant_sqr();
        // w_en_psst = false;
        clear_en_passant_sqr(move.start.file, move.start.rank + 1);
    }
    else if (!w_ply && is_en_passant_sqr(move.start.file, move.start.rank - 1)) {
        // black.reset_en_passant_sqr();
        // b_en_psst = false;
        clear_en_passant_sqr(move.start.file, move.start.rank - 1);
    }

    p->updt_position(move.start.file, move.start.rank, true);
    p->reveal();

    if (move.prom != blank) {
        if (w_ply)
            white.piece_captured(move.target.file, move.target.rank);
        else
            black.piece_captured(move.target.file, move.target.rank);
    }
    
    // if (w_ply)
    //     for (size_t i(0); i < black.get_nb_pieces(); ++i) {
    //         black.get_piece(i)->reveal();
    //     }
    // else
    //     for (size_t i(0); i < white.get_nb_pieces(); ++i) {
    //         white.get_piece(i)->reveal();
    //     }

    updt_board();
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
    
    bool cap(is_enemy(p->get_code(), trgt_file, trgt_rank)
             || (is_en_passant_sqr(trgt_file, trgt_rank)
             && (p->get_code() == 'P' || p->get_code() == 'p')));

    if (handle_check(p, trgt_file, trgt_rank, cap, w_ply))
        return true;

    return false;
}

bool Game::handle_check(Piece* piece, char trgt_file, int trgt_rank, bool capt,bool w_p) {

    Square start_sqr({piece->get_file(), piece->get_rank()});
    // Check that castling doesn't cross a check
    bool castling_cross_chk(false);
    if (w_p && k_castle) {
        for (char f(piece->get_file()); f < 'h'; ++f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (black.attacker()) {
                castling_cross_chk = true;
                break;
            }
        }
    }else if (w_p && q_castle) {
        for (char f(piece->get_file()); f > 'b'; --f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (black.attacker()) {
                castling_cross_chk = true;
                break;
            }
        }
    }else if (!w_p && k_castle) {
        for (char f(piece->get_file()); f < 'h'; ++f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (white.attacker()) {
                castling_cross_chk = true;
                break;
            }
        }
    }else if (!w_p && q_castle) {
        for (char f(piece->get_file()); f > 'b'; --f) {
            piece->updt_position(f, trgt_rank, true);
            updt_board();
            if (white.attacker()) {
                castling_cross_chk = true;
                break;
            }
        }
    }
    if (castling_cross_chk) {
        piece->updt_position(start_sqr.file, start_sqr.rank, true);
        updt_board();
        return false;
    }

    // Silently execute the move to see if it leads to a rule violation.
    bool trgt_is_ep_sqr(is_en_passant_sqr(trgt_file, trgt_rank));
    piece->updt_position(trgt_file, trgt_rank, true);

    bool illegal(false);
    if (capt) {
        if (trgt_is_ep_sqr) {
            if (w_p) {
                black.hide_piece(trgt_file, trgt_rank-1);
                updt_board();
                if (black.attacker() != nullptr)
                    illegal = true;
                black.reveal_piece(trgt_file, trgt_rank-1);
            }else {
                white.hide_piece(trgt_file, trgt_rank+1);
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
    
    if (w_p && white.has_en_passant_sqr()) {
        clear_en_passant_sqr(trgt_file, trgt_rank-1);
        white.reset_en_passant_sqr();
    }else if (!w_p && black.has_en_passant_sqr()) {
        clear_en_passant_sqr(trgt_file, trgt_rank+1);
        black.reset_en_passant_sqr();
    }

    if (trgt_is_ep_sqr)
        write_en_passant_sqr(trgt_file, trgt_rank);
    updt_board();

    return !illegal;
}

bool Game::process_move(Piece* p, char trgt_file, int trgt_rank, bool w_ply, 
                                                char prom_piece, bool test) {

    bool ep_cap(is_en_passant_sqr(trgt_file, trgt_rank)
                && (p->get_code() == 'P' || p->get_code() == 'p'));
    bool cap(is_enemy(w_ply, trgt_file, trgt_rank) || ep_cap);
    capture = cap;

    last_move = current_move;
    // Save in memory the start and target squares of the active piece
    start = {p->get_file(), p->get_rank()};
    target = {trgt_file, trgt_rank};
    current_move = {p->get_code(), start, target, prom_piece};
    if (!w_ply)
        current_move.piece += upcase_shift;

    

    if (is_any_en_psst_sqr()) {
        Square ep_sqr(get_en_passant_sqr());
        if (!w_ply && ep_sqr.rank == 3)
            clear_en_passant_sqr(ep_sqr.file, ep_sqr.rank);
        else if (w_ply && ep_sqr.rank == 6)
            clear_en_passant_sqr(ep_sqr.file, ep_sqr.rank);
    }
    // Update position and in-range squares of the active piece
    p->updt_position(trgt_file, trgt_rank, test);
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
    // if (white.has_en_passant_sqr())
    //     w_en_psst = true;
    // else if (black.has_en_passant_sqr())
    //     b_en_psst = true;

    // Delete the captured piece if there is any.
    // Hide it, i.e. make it inactive and unseeable while keeping it in memory
    // when this function is called by 'compute_moves'
    if (cap) {
        if (w_ply) {
            if (ep_cap) {
                ++t_result.n_ep;
                // std::wcout << "epcap\n";
                if (test)
                    black.hide_piece(trgt_file, trgt_rank-1);
                else
                    black.piece_captured(trgt_file, trgt_rank-1);
            }else {
                ++t_result.n_cptrs;
                if (test)
                    black.hide_piece(trgt_file, trgt_rank);
                else
                    black.piece_captured(trgt_file, trgt_rank);
            }
        }else {
            if (ep_cap) {
                ++t_result.n_ep;
                if (test)
                    white.hide_piece(trgt_file, trgt_rank+1);
                else
                    white.piece_captured(trgt_file, trgt_rank+1);
            }else {
                ++t_result.n_cptrs;
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
        white.new_piece(prom_piece + upcase_shift, trgt_file, trgt_rank);
    }else if (!w_ply && p->get_code() == 'p' && trgt_rank == 1) {
        ++t_result.n_proms;
        if (test)
            black.hide_piece(trgt_file, trgt_rank);
        else
            black.piece_captured(trgt_file, trgt_rank);
        black.new_piece(prom_piece, trgt_file, trgt_rank);
    }

    if (w_ply) {
        if (white.attacker()) {
            check = true;
            ++t_result.n_chks;
        }else {
            check = false;
            if (SAN_chk)
                std::wcout << msg_color << "this is not a check\n" << reset_sgr;
        }
    }else {
        if (black.attacker()) {
            check = true;
            ++t_result.n_chks;
        }else {
            check = false;
            if (SAN_chk)
                std::wcout << msg_color << "This is not a check\n" << reset_sgr;
        }
    }

    // if (w_en_psst && !w_ply) {
    //     clear_en_passant_sqr();
    //     white.reset_en_passant_sqr();
    //     w_en_psst = false;
    // }else if (b_en_psst && w_ply) {
    //     clear_en_passant_sqr();
    //     black.reset_en_passant_sqr();
    //     b_en_psst = false;
    // }
    
    if (!test) {
        if (check) {
            if (is_checkmate(w_ply)) {
                checkmate = true;
                message::write_to_log(current_move, w_ply, nb_move, capture, check,
                                                                             checkmate);
                message::checkmate(w_ply);
                updt_board();
                print_position(!w_ply);
                ++t_result.n_chkmates;
                --t_result.n_chks;
                return true;
            }
        }else if (is_draw(w_ply)) {
            updt_board();
            print_position(!w_ply);
            return true;
        }
    }

    if (!test)
        message::write_to_log(current_move, w_ply, nb_move, capture, check, checkmate);

    // if (test && !w_ply)
    //     print_position(true);
    updt_board();
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
                if (is_enemy(piece->get_code(), sq.file, sq.rank)
                    || is_en_passant_sqr(sq.file, sq.rank))
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
                if (is_enemy(piece->get_code(), sq.file, sq.rank)
                    || is_en_passant_sqr(sq.file, sq.rank))
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
    const std::wstring prompt = cursor_up + clr_line + italic +
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

    std::vector<Move> moves(generate_legal_moves(w_turn));

    size_t i(rand() % moves.size());
    Move move(moves[i]);
    if (w_turn)
        p = white.find_piece(move.piece, move.start.file, move.start.rank);
    else
        p = black.find_piece(move.piece, move.start.file, move.start.rank);
    if (process_move(p, move.target.file, move.target.rank, w_turn, move.prom))
        return true;
    
    w_turn = !w_turn;
    return false;
}

void Game::test_gen_moves(int max_depth) {

#ifndef DIVIDE
    for (int i(1); i <= max_depth; ++i) {
        t_result = {0, 0, 0, 0, 0};
        perft_depth = i;

        std::wcout << "depth: " << i
                   << "\tnodes: " << save_cursor_pos << msg_color
                   << compute_moves(i, w_turn) << reset_sgr;
    #ifndef BULK_COUNTING
        std::wcout << "\tcaptures: " << msg_color << t_result.n_cptrs << reset_sgr
                   << "\te-p: " << msg_color << t_result.n_ep << reset_sgr
                   << "\tcastles: " << msg_color << t_result.n_cstls << reset_sgr
                   << "\tpromotions: " << msg_color << t_result.n_proms << reset_sgr
                   << "\tchecks: " << msg_color << t_result.n_chks << reset_sgr
                   << "\tcheckmates: " << msg_color << t_result.n_chkmates << reset_sgr;
    #endif // BULK_COUNTING
        std::wcout << "\n";
    }
#else
    perft_depth = max_depth;
    std::wcout << divide(max_depth, w_turn) << "\n\n";
#endif // DIVIDE
}

int Game::divide(int depth, bool w_ply) {

    std::vector<Move> moves(generate_moves(w_ply));
    Piece* p(nullptr);
    int n_positions(0);
    for (auto move : moves) {

        std::wcout << move.start.file << move.start.rank
                   << move.target.file << move.target.rank;
        if (move.prom != blank)
            std::wcout << move.prom;
        std::wcout << ": ";
        if (depth == 1) {
            std::wcout << "1\n";
            continue;
        }

        bool k_cstl(false), q_cstl(false);
        if (w_ply)
            p = white.find_piece(move.piece, move.start.file, move.start.rank);
        else
            p = black.find_piece(move.piece, move.start.file, move.start.rank);

        Square enemy_ep_sqr;
        bool is_enemy_ep_sqr(false);
        if (is_any_en_psst_sqr()) {
            enemy_ep_sqr = {get_en_passant_sqr().file, get_en_passant_sqr().rank};
            is_enemy_ep_sqr = true;
        }

        bool cap(is_enemy(w_ply, move.target.file, move.target.rank));
        bool ep_cap(is_en_passant_sqr(move.target.file, move.target.rank)
                    && (p->get_code() == 'P' || p->get_code() == 'p'));

        make_move(p, move, w_ply, k_cstl, q_cstl);
        if (is_move_legal(p, move.target.file, move.target.rank, w_ply)) {
            int n_nodes(compute_moves(depth-1, !w_ply));
            n_positions += n_nodes;
            std::wcout << n_nodes << "\n";
        }
        unmake_move(p, move, w_ply, k_cstl, q_cstl);

        if (is_enemy_ep_sqr)
            write_en_passant_sqr(enemy_ep_sqr.file, enemy_ep_sqr.rank);

        if (cap) {
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank);
            }else {
                white.reveal_piece(move.target.file, move.target.rank);
            }
        }else if (ep_cap) {
            write_en_passant_sqr(move.target.file, move.target.rank);
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank-1);
            }else {
                white.reveal_piece(move.target.file, move.target.rank+1);
            }
        }
        updt_board();
    }
    std::wcout << "\nNodes searched: ";
    if (depth == 1)
        return moves.size();
    return n_positions;
}

int Game::compute_moves(int depth, bool w_ply) {

#ifdef BULK_COUNTING
    // if (depth == 1)
    //     print_position(true);
    std::vector<Move> moves(generate_legal_moves(w_ply));
    if (depth == 1) {
        // std::wcout << restore_cursor_pos << clr_end_line;
        return moves.size();
    }
#else
    if (depth == 0) {
        return 1;
    }
    std::vector<Move> moves(generate_legal_moves(w_ply));
#endif // BULK_COUNTING

    Piece* p(nullptr);
    int n_positions(0);
    size_t i(0);

    for (auto move : moves) {        
        if (depth == perft_depth) {
            std::wcout << restore_cursor_pos << clr_end_line << msg_color
                       << ++i << "/" << moves.size() << "\n";
            if (i == moves.size())
                std::wcout << restore_cursor_pos << clr_end_line << msg_color;
        }

        // std::wcout << move.start.file << move.start.rank
        //            << move.target.file << move.target.rank << "\n";

        bool k_cstl(false), q_cstl(false);

        if (w_ply)
            p = white.find_piece(move.piece, move.start.file, move.start.rank);
        else
            p = black.find_piece(move.piece, move.start.file, move.start.rank);

        Square enemy_ep_sqr;
        bool is_enemy_ep_sqr(false);
        if (is_any_en_psst_sqr()) {
            enemy_ep_sqr = {get_en_passant_sqr().file, get_en_passant_sqr().rank};
            is_enemy_ep_sqr = true;
        }

        bool cap(is_enemy(w_ply, move.target.file, move.target.rank));
        bool ep_cap(is_en_passant_sqr(move.target.file, move.target.rank)
                    && (p->get_code() == 'P' || p->get_code() == 'p'));

        make_move(p, move, w_ply, k_cstl, q_cstl);
        // if (is_move_legal(p, move.target.file, move.target.rank, w_ply))
        n_positions += compute_moves(depth-1, !w_ply);
        unmake_move(p, move, w_ply, k_cstl, q_cstl);

        if (is_enemy_ep_sqr)
            write_en_passant_sqr(enemy_ep_sqr.file, enemy_ep_sqr.rank);

        if (cap) {
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank);
            }else {
                white.reveal_piece(move.target.file, move.target.rank);
            }
        }else if (ep_cap) {
            write_en_passant_sqr(move.target.file, move.target.rank);
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank-1);
            }else {
                white.reveal_piece(move.target.file, move.target.rank+1);
            }
        }
        updt_board();
    }
 
    return n_positions;
}

int Game::search(int depth, int alpha, int beta, bool w_ply) {
    std::vector<Move> moves(generate_legal_moves(w_ply));
    if (depth == 1) {
        // std::wcout << restore_cursor_pos << clr_end_line;
        return evaluate(w_ply);
    }

    if (moves.empty()) {
        if (check)
            return -1000;
        return 0;
    }

    Piece* p(nullptr);
    size_t i(0);

    for (auto move : moves) {        
        if (depth == perft_depth) {
            std::wcout << restore_cursor_pos << clr_end_line << msg_color
                       << ++i << "/" << moves.size() << "\n";
            if (i == moves.size())
                std::wcout << restore_cursor_pos << clr_end_line << msg_color;
        }

        if (w_ply)
            p = white.find_piece(move.piece, move.start.file, move.start.rank);
        else
            p = black.find_piece(move.piece, move.start.file, move.start.rank);

        Square enemy_ep_sqr;
        bool is_enemy_ep_sqr(is_any_en_psst_sqr());
        if (is_enemy_ep_sqr)
            enemy_ep_sqr = {get_en_passant_sqr().file, get_en_passant_sqr().rank};

        bool k_cstl(false), q_cstl(false);
        bool cap(is_enemy(w_ply, move.target.file, move.target.rank));
        bool ep_cap(is_en_passant_sqr(move.target.file, move.target.rank)
                    && (p->get_code() == 'P' || p->get_code() == 'p'));

        make_move(p, move, w_ply, k_cstl, q_cstl);
        int evaluation(-search(depth-1, -beta, -alpha, !w_ply));
        unmake_move(p, move, w_ply, k_cstl, q_cstl);
        if (evaluation >= beta)
            return beta;
        alpha = std::max(alpha, evaluation);

        if (is_enemy_ep_sqr)
            write_en_passant_sqr(enemy_ep_sqr.file, enemy_ep_sqr.rank);

        if (cap) {
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank);
            }else {
                white.reveal_piece(move.target.file, move.target.rank);
            }
        }else if (ep_cap) {
            write_en_passant_sqr(move.target.file, move.target.rank);
            if (w_ply) {
                black.reveal_piece(move.target.file, move.target.rank-1);
            }else {
                white.reveal_piece(move.target.file, move.target.rank+1);
            }
        }
        updt_board();
    }
    return alpha;
}

int Game::evaluate(bool w_ply) {
    int w_material(count_material(true));
    int b_material(count_material(false));

    return (w_ply ? w_material - b_material : b_material - w_material);
}

int Game::count_material(bool w_ply) {
    int material(0);
    material += piece_occurences(w_ply ? 'P' : 'p');
    material += piece_occurences(w_ply ? 'N' : 'n');
    material += piece_occurences(w_ply ? 'B' : 'b');
    material += piece_occurences(w_ply ? 'R' : 'r');
    material += piece_occurences(w_ply ? 'Q' : 'q');
    return material;
}

std::vector<Move> Game::generate_moves(bool w_ply) {

    size_t n;
    Piece* p(nullptr);
    std::vector<Move> pseudo_legal_moves;

    if (w_ply)
        n = white.get_nb_pieces();
    else
        n = black.get_nb_pieces();

    for (size_t i(0); i < n; ++i) {
        p = (w_ply ? white.get_piece(i) : black.get_piece(i));
        if (p->get_hidden())
            continue;
        
        for (auto& sq : p->get_cov_sqrs()) {
            if ((w_ply && p->get_code() == 'P' && sq.rank == 8)
                || (!w_ply && p->get_code() == 'p' && sq.rank == 1)) {

                pseudo_legal_moves.push_back(Move(p->get_code(),
                                            {p->get_file(), p->get_rank()},
                                            {sq.file, sq.rank},
                                            'q'));
                pseudo_legal_moves.push_back(Move(p->get_code(),
                                            {p->get_file(), p->get_rank()},
                                            {sq.file, sq.rank},
                                            'r'));
                pseudo_legal_moves.push_back(Move(p->get_code(),
                                            {p->get_file(), p->get_rank()},
                                            {sq.file, sq.rank},
                                            'b'));
                pseudo_legal_moves.push_back(Move(p->get_code(),
                                            {p->get_file(), p->get_rank()},
                                            {sq.file, sq.rank},
                                            'n'));
            }else if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                pseudo_legal_moves.push_back(Move(p->get_code(),
                                           {p->get_file(), p->get_rank()},
                                           {sq.file, sq.rank},
                                           blank));
            }
        }
    }
    return pseudo_legal_moves;
}

std::vector<Move> Game::generate_legal_moves(bool w_ply) {

    size_t n;
    Piece* p(nullptr);
    std::vector<Move> legal_moves;

    if (w_ply)
        n = white.get_nb_pieces();
    else
        n = black.get_nb_pieces();

    for (size_t i(0); i < n; ++i) {

        if (w_ply)
            p = white.get_piece(i);
        else
            p = black.get_piece(i);
        
        if (p->get_hidden())
            continue;
        
        for (auto& sq : p->get_cov_sqrs()) {
            k_castle = false;
            q_castle = false;
            // std::wcout << p->get_file() << p->get_rank() << sq.file << sq.rank << "\n";
            
            if ((w_ply && p->get_code() == 'K') || (!w_ply && p->get_code() == 'k')) {
                if (sq.file == p->get_file() + 2)
                    k_castle = true;
                else if (sq.file == p->get_file() - 2)
                    q_castle = true;
            }

            if ((w_ply && p->get_code() == 'P' && sq.rank == 8)
                || (!w_ply && p->get_code() == 'p' && sq.rank == 1)) {

                if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                    legal_moves.push_back(Move(p->get_code(),
                                               {p->get_file(), p->get_rank()},
                                               {sq.file, sq.rank},
                                               'q'));
                    legal_moves.push_back(Move(p->get_code(),
                                               {p->get_file(), p->get_rank()},
                                               {sq.file, sq.rank},
                                               'r'));
                    legal_moves.push_back(Move(p->get_code(),
                                               {p->get_file(), p->get_rank()},
                                               {sq.file, sq.rank},
                                               'b'));
                    legal_moves.push_back(Move(p->get_code(),
                                               {p->get_file(), p->get_rank()},
                                               {sq.file, sq.rank},
                                               'n'));
                }
            }else if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                legal_moves.push_back(Move(p->get_code(),
                                           {p->get_file(), p->get_rank()},
                                           {sq.file, sq.rank},
                                           blank));
            }
        }
    }
    // for (auto move : legal_moves) {
    //    std::wcout << move.start.file << move.start.rank
    //               << move.target.file << move.target.rank << "\n";
    // }

    return legal_moves;
}
