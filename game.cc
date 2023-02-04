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
#include "view.h"
#include "common.h"
#include "game.h"

Game::Game()
    :   w_turn(true), capture(false), promote(false), q_castle(false), 
        k_castle(false), check(false), SAN_chk(false), SAN_cap(false),
        w_en_psst(false), b_en_psst(false), SAN_file(blank), SAN_rank(blank), SAN_spec_file(blank), SAN_spec_rank(blank), attacker(nullptr) {}

Game::~Game() {}

void Game::game_flow(bool black, bool pvp, bool cvc) {

    updt_board();
    fen_verify_checks();
    print_position();

    std::wstring SAN;
    while (true) {
        if (black) {
            bool game_over(computer_play());
            if(game_over)
                break;
            updt_board();
            print_position();
        }
        if (!cvc) {
            prompt_move();
            std::wcin >> SAN;
            if (parse_cmd(SAN))
                continue;

            if (is_SAN_valid(SAN)) {
                // std::wcout << clr_display;
                int result(process_move());
                if (result == 1)
                    break;
                else if (result == 2)
                    continue;
                else {
                    updt_board();
                    if (pvp)
                        print_position();
                }
            }
            else {
                std::wcout << INVALID_SAN;
                continue;
            }
            if (pvp)
                continue;
        }

        if (!black) {
            bool game_over(computer_play());
            if(game_over)
                break;
            updt_board();
            print_position();
        }
    }
}

void Game::prompt_move() {
    if (w_turn) std::wcout << "\n\x1b[3m" "White to play: " << reset_sgr;
    else std::wcout << "\n\x1b[3m" "Black to play: " << reset_sgr;
}

void Game::print_position() {
    board_print_board(w_turn, start, target, check);
    // print_ascii();
}

void Game::updt_board() {
    empty_board();
    white.write_pieces_on_board();
    black.write_pieces_on_board();
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
                std::wcout << FEN_ERROR;
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
        print_position();
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
    prom_piece = blank;
    promote = false;
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
        
    if (promote && (w_turn ? SAN_rank != '8' : SAN_rank != '1')) 
        return false;
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
            }
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
                prom_piece = c;
            else 
                return false;
            break;
        case 'x':
            SAN_cap = true;
            if (prev >= 'a' && prev <= 'h')
                SAN_spec_file = prev;
            break;
        case '=':
            promote = true;
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

int Game::process_move() {
    // Raise non-sense exception.
    // (i.e. you enter 'dxe4' but there is no enemy on e4).
    if (SAN_cap && !is_enemy(w_turn, SAN_file, SAN_rank - '0') 
        && !is_en_passant_sqr(SAN_file, SAN_rank - '0')) 
    {
        std::wcout << msg_color << NOT_A_CAPTURE << SAN_file << SAN_rank
                   << "\n" << reset_sgr;
        return 0;
    }

    Piece* matched_piece(nullptr);

    // Handle castling.
    bool castling_ok(false);
    if (k_castle || q_castle) {
        if (process_castling())
            castling_ok = true;
    }
    else if (w_turn)
        matched_piece = white.unique_piece_for_move(SAN_piece, SAN_file, SAN_rank,
                                                    SAN_spec_file, SAN_spec_rank);
    else
        matched_piece = black.unique_piece_for_move(SAN_piece, SAN_file, SAN_rank,
                                                    SAN_spec_file, SAN_spec_rank);

    // A piece can't move if it makes the king under check.
    // Handle pins, simple checks and double checks.
    if (matched_piece != nullptr) {
        capture = false;
        if (is_enemy(w_turn, SAN_file, SAN_rank-'0')
            || is_en_passant_sqr(SAN_file, SAN_rank-'0'))
            capture = true;
        if (!handle_check(matched_piece, SAN_file, SAN_rank - '0', capture, w_turn)) {
            matched_piece = nullptr;
            std::wcout << "  " << ILLEGAL << "\n";
        }
    }

    if (matched_piece != nullptr) {
        // Update position and in-range squares of the active piece.
        matched_piece->updt_position(SAN_file, SAN_rank);
        matched_piece->updt_cov_sqrs();
        updt_board();

        if (white.has_en_passant_sqr())
            w_en_psst = true;
        else if (black.has_en_passant_sqr()) {
            b_en_psst = true;
        }

        // Save in memory the start and target squares of the active piece.
        if (w_turn) 
            start = white.get_start_sqr();
        else 
            start = black.get_start_sqr();
        target = {SAN_file, SAN_rank - '0'};

        // Delete the captured piece from memory in case of a capture.
        if (capture) {
            if ((w_en_psst || b_en_psst) && !is_enemy(w_turn, SAN_file, SAN_rank - '0')) {
                if (w_turn) {
                    black.piece_captured(SAN_file, SAN_rank-'0' - 1);
                }
                else 
                    white.piece_captured(SAN_file, SAN_rank-'0' + 1);
            }else {
                if (w_turn)
                    black.piece_captured(SAN_file, SAN_rank);
                else 
                    white.piece_captured(SAN_file, SAN_rank);
            }
        }

        if (promote) {
            if (w_turn) {
                white.piece_captured(SAN_file, SAN_rank);
                white.new_piece(prom_piece, SAN_file, SAN_rank - '0');
            }
            else {
                black.piece_captured(SAN_file, SAN_rank);
                black.new_piece(prom_piece - upcase_shift, SAN_file, SAN_rank - '0');
            }
            updt_board();
        }else if (w_turn && matched_piece->get_code() == 'P' && SAN_rank == '8') {
            white.piece_captured(SAN_file, SAN_rank - '0');
            white.new_piece('Q', SAN_file, SAN_rank - '0');
        }else if (!w_turn && matched_piece->get_code() == 'p' && SAN_rank == '1') {
            black.piece_captured(SAN_file, SAN_rank - '0');
            black.new_piece('q', SAN_file, SAN_rank - '0');
        }

        // Save in memory what piece is attacking the king in case of a check.
        if (w_turn) {
            if (white.attacker() != nullptr) {
                attacker = white.attacker();
                check = true;
            }else {
                attacker = nullptr;
                check = false;
                if (SAN_chk) 
                    std::wcout << "  " << msg_color <<  NOT_A_CHECK << reset_sgr;
            }
        }else {
            if (black.attacker() != nullptr) {
                attacker = black.attacker();
                check = true;
            }else {
                attacker = nullptr;
                check = false;
                if (SAN_chk) 
                    std::wcout << "  " << msg_color << NOT_A_CHECK << reset_sgr;
            }
        }
        // Reset en passant.
        if (w_en_psst && !w_turn) {
            clear_en_passant_sqr();
            white.reset_en_passant_sqr();
            w_en_psst = false;
        }else if (b_en_psst && w_turn) {
            clear_en_passant_sqr();
            black.reset_en_passant_sqr();
            b_en_psst = false;
        }
    }
    if (matched_piece == nullptr && !castling_ok)
        return 2;

    // If the move is valid, it's the other color to play.
    if (matched_piece != nullptr || castling_ok) 
        w_turn = 1 - w_turn;

    // Exit the main loop when the game ends.
    capture = false;
    if (check) {
        if (is_checkmate()) {
            std::wcout << "  " << msg_color << CHECKMATE << "  ";
            std::wcout << (w_turn ? BLACK_WINS : WHITE_WINS) << reset_sgr;
            updt_board();
            print_position();
            return 1;
        }
    }else if (is_draw()) {
        std::wcout << msg_color << DRAW << reset_sgr;
        updt_board();
        print_position();
        return 1;
    }
    return 0;
}

bool Game::process_castling() {
    if (w_turn) {
        if (k_castle) {
            if (white.can_k_castle()) {
                white.castle_king_side();
                start = white.get_start_sqr();
                target = {'e', 1};
            }else {
                std::wcout << ILLEGAL << "\n";
                return false;
            }
        }
        else if (q_castle) {
            if (white.can_q_castle()) {
                white.castle_queen_side();
                start = white.get_start_sqr();
                target = {'e', 1};
            }else {
                std::wcout << ILLEGAL << "\n";
                return false;
            }
        }
    }
    else {
        if (k_castle) {
            if (black.can_k_castle()) {
                black.castle_king_side();
                start = black.get_start_sqr();
                target = {'e', 8};
            }else {
                std::wcout << ILLEGAL << "\n";
                return false;
            }
        }
        else if (q_castle) {
            if (black.can_q_castle()) {
                black.castle_queen_side();
                start = black.get_start_sqr();
                target = {'e', 8};
            }else {
                std::wcout << ILLEGAL << "\n";
                return false;
            }
        }
    }
    return true;
}

bool Game::handle_check(Piece* piece, char trgt_file, int trgt_rank, bool capt,bool w_p) {
    // Silently execute the move to see if it leads to a rule violation.
    Square start_sqr({piece->get_file(), piece->get_rank()});
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
            || (!w_p && white.attacker() != nullptr))
            illegal = true;
    }
    // Replace the piece on its square.
    piece->updt_position(start_sqr.file, start_sqr.rank, true);
    if (en_psst)
        write_en_passant_sqr(trgt_file, trgt_rank);
    updt_board();
    return !illegal;
}

bool Game::is_checkmate() {
    Piece* piece(nullptr);
    bool cap(false);
    if (w_turn) {
        size_t nb(white.get_nb_pieces());
        for (size_t i(0); i < nb; ++i) {
            piece = white.get_piece(i);
            for (auto sq : piece->get_cov_sqrs()) {
                cap = false;
                if (is_enemy(piece->get_code(), sq.file, sq.rank))
                    cap = true;
                if (handle_check(piece, sq.file, sq.rank, cap, w_turn))
                    return false;
            } 
        }
    }else {
        size_t nb(black.get_nb_pieces());
        for (size_t i(0); i < nb; ++i) {
            piece = black.get_piece(i);
            for (auto sq : piece->get_cov_sqrs()) {
                cap = false;
                if (is_enemy(piece->get_code(), sq.file, sq.rank))
                    cap = true;
                if (handle_check(piece, sq.file, sq.rank, cap, w_turn))
                    return false;
            }
        }
    }
    return true;
}

bool Game::is_draw() {
    // Draw if only the 2 kings remain on the board.
    if (white.king_is_last() && black.king_is_last()) {
        std::wcout << "  " << msg_color << NO_WIN << "  " << reset_sgr;
        return true;
    }
    // Stalemate
    if (is_checkmate()) {
        std::wcout << "  " << msg_color << STALEMATE << "  " << reset_sgr;
        return true;
    }
    return false;
}

int Game::is_resign() {
    const std::wstring prompt = erase_line + italic +
                   L"Do you really want to resign ? [Y/n] " + reset_sgr;
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
        std::wcout << msg_color << WHITE_RESIGNS << "  " << BLACK_WINS;
    else
        std::wcout << msg_color << BLACK_RESIGNS << "   " << WHITE_WINS;
    exit(0);
}

bool Game::computer_play() {
    Piece* p(nullptr);
    if (w_turn) {
        char sf;
        int sr;
        size_t n(white.get_nb_pieces());
        while (true) {
            do {
                size_t i(rand() % n);
                p = white.get_piece(i);
                sf = p->get_file();
                sr = p->get_rank();
                p->updt_cov_sqrs();
            } while (p->get_hidden() || p->get_cov_sqrs().size() == 0);
            
            size_t nsq(p->get_cov_sqrs().size());
            size_t i(rand() % nsq);
            char tf(p->get_cov_sqrs()[i].file);
            int tr(p->get_cov_sqrs()[i].rank);
            if (is_move_legal(p, tf, tr, w_turn)) {
                if (process_move(p, tf, tr, w_turn))
                    return true;
                start = {sf, sr};
                break;
            }
        }
    }else {
        size_t n(black.get_nb_pieces());
        char sf;
        int sr;
        while (true) {
            do {
                size_t i(rand() % n);
                p = black.get_piece(i);
                sf = p->get_file();
                sr = p->get_rank();
                p->updt_cov_sqrs();
            } while (p->get_hidden() || p->get_cov_sqrs().size() == 0);

            size_t nsq(p->get_cov_sqrs().size());
            size_t i(rand() % nsq);
            char tf(p->get_cov_sqrs()[i].file);
            int tr(p->get_cov_sqrs()[i].rank);
            if (is_move_legal(p, tf, tr, w_turn)) {
                if (process_move(p, tf, tr, w_turn))
                    return true;
                start = {sf, sr};
                break;
            }
        }
    }
    w_turn = 1 - w_turn;
    return false;
}

void Game::test_gen_moves(int max_depth) {
    for (int i(1); i <= max_depth; ++i) {
        std::wcout << "plies: " << i << "\tpositions: "
                   << msg_color << compute_moves(i, true) << reset_sgr << "\n";
    }
}

int Game::compute_moves(int depth, bool w_ply) {
    if (depth == 0) {
        // std::wcout << "+1\n";
        return 1;
    }
    
    int n_positions(0);
    Piece* p(nullptr);
    if (w_ply) {
        size_t n(white.get_nb_pieces());
        for (size_t i(0); i < n; ++i) {
            p = white.get_piece(i);
            if (p->get_hidden())
                continue;
            p->updt_cov_sqrs();
            for (auto& sq : p->get_cov_sqrs()) {
                if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                    char sf(p->get_file());
                    int sr(p->get_rank());
                    process_move(p, sq.file, sq.rank, w_ply);
                    n_positions += compute_moves(depth - 1, !w_ply);
                    p->updt_position(sf, sr);
                    p->reveal();
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
            p->updt_cov_sqrs();
            for (auto& sq : p->get_cov_sqrs()) {
                if (is_move_legal(p, sq.file, sq.rank, w_ply)) {
                    char sf(p->get_file());
                    int sr(p->get_rank());
                    process_move(p, sq.file, sq.rank, w_ply);
                    n_positions += compute_moves(depth - 1, !w_ply);
                    p->updt_position(sf, sr);
                    p->reveal();
                    updt_board();
                }
            }
        }
    }
    return n_positions;
}

bool Game::is_move_legal(Piece* p, char trgt_file, int trgt_rank, bool w_ply) {
    capture = false;
    if (is_enemy(p->get_code(), trgt_file, trgt_rank)
        || is_en_passant_sqr(trgt_file, trgt_rank))
        capture = true;
    if (handle_check(p, trgt_file, trgt_rank, capture, w_ply))
        return true;
    return false;
}

bool Game::process_move(Piece* p, char trgt_file, int trgt_rank, bool w_ply) {
    capture = false;
    if (is_enemy(p->get_code(), trgt_file, trgt_rank)
        || is_en_passant_sqr(trgt_file, trgt_rank))
        capture = true;
    
    p->updt_position(trgt_file, trgt_rank);
    p->updt_cov_sqrs();
    updt_board();

    if (white.has_en_passant_sqr())
        w_en_psst = true;
    else if (black.has_en_passant_sqr())
        b_en_psst = true;

    target = {trgt_file, trgt_rank};

    if (capture) {
        // std::wcout << "captured\n";
        if ((w_en_psst || b_en_psst) && !is_enemy(w_ply, trgt_file, trgt_rank)) {
            if (w_ply) {
                black.hide_piece(trgt_file, trgt_rank - 1);
            }
            else {
                white.hide_piece(trgt_file, trgt_rank + 1);
            }
        }else {
            if (w_ply) {
                black.hide_piece(trgt_file, trgt_rank);
                updt_board();
            }
            else {
                white.hide_piece(trgt_file, trgt_rank);
                updt_board();
            }
        }
    }

    if (w_ply && p->get_code() == 'P' && trgt_rank == 8) {
        std::wcout << p->get_code() << "\n";
        white.piece_captured(trgt_file, trgt_rank);
        white.new_piece('Q', trgt_file, trgt_rank);
        updt_board();
    }else if (!w_ply && p->get_code() == 'p' && trgt_rank == 1) {
        black.piece_captured(trgt_file, trgt_rank);
        black.new_piece('q', trgt_file, trgt_rank);
        updt_board();
    }

    if (w_ply) {
        if (white.attacker()) {
            attacker = white.attacker();
            check = true;
        }else {
            attacker = nullptr;
            check = false;
        }
    }else {
        if (black.attacker()) {
            attacker = black.attacker();
            check = true;
        }else {
            attacker = nullptr;
            check = false;
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
    w_turn = 1 - w_turn;
    capture = false;
    if (check) {
        if (is_checkmate()) {
            std::wcout << "  " << msg_color << CHECKMATE << "  ";
            std::wcout << (w_turn ? BLACK_WINS : WHITE_WINS) << reset_sgr;
            updt_board();
            print_position();
            return true;
        }
    }else if (is_draw()) {
        std::wcout << msg_color << DRAW << reset_sgr;
        updt_board();
        print_position();
        return true;
    }
    w_turn = 1 - w_turn;
    return false;
}