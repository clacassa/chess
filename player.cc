/*
 *  player.cc -- chess -- Chess game in the terminal
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
#include "player.h"
#include "piece.h"
#include "board.h"
#include "message.h"

Piece* Player::unique_piece_for_move(char SAN_piece, char SAN_file, char SAN_rank,
                                     char SAN_spec_file, char SAN_spec_rank) {
    elligible_pieces.clear();
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        p->updt_cov_sqrs();
        if (p->is_elligible_for_move(SAN_piece, SAN_file, SAN_rank, 
                                     SAN_spec_file, SAN_spec_rank)) {
            elligible_pieces.push_back(p);
        }
    }
    if (elligible_pieces.size() == 1)
        return elligible_pieces[0];

    if (elligible_pieces.size() == 0)
        message::illegal_move(SAN_file, SAN_rank - '0', SAN_piece);

    else if (elligible_pieces.size() > 1)
        message::ambigueous_move(SAN_file, SAN_rank-'0', SAN_piece);

    return nullptr;
}

Piece* Player::attacker() {
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        p->updt_cov_sqrs();
        if (p->attacking_enemy_king())
            return p;
    }
    return nullptr;
}

void Player::write_pieces_on_board() {
    for (auto p : pieces) {
        if (p->get_hidden())
            continue;
        write_piece(p->get_code(), p->get_file(), p->get_rank());
    }
}

// void Player::new_piece(char code, char file, char rank) {
//     switch (code) {
//         case 'B': pieces.push_back(new Bishop(code, file, rank)); break;
//         case 'K': pieces.push_back(new King(code, file, rank)); break;
//         case 'N': pieces.push_back(new Knight(code, file, rank)); break;
//         case 'P': pieces.push_back(new Pawn(code, file, rank)); break;
//         case 'Q': pieces.push_back(new Queen(code, file, rank)); break;
//         case 'R': pieces.push_back(new Rook(code, file, rank)); break;
//         default: std::wcout << "Couldn't add a new piece to a player\n"; break;
//     }
// }

void Player::reveal_piece(char file, int rank) {
    for (auto& p : pieces) {
        if (p->get_file() == file && p->get_rank() == rank)
            p->reveal();
    }
}

void Player::hide_piece(char file, int rank) {
    for (auto& p : pieces) {
        if (p->get_file() == file && p->get_rank() == rank) {
            p->hide();
        }
    }
}

void Player::piece_captured(char SAN_file, char SAN_rank) {
    for (size_t i(0); i < pieces.size(); ++i) {
        if (pieces[i]->get_file() == SAN_file && 
            pieces[i]->get_rank() == SAN_rank - '0') {
            last_captured = *pieces[i];
            delete pieces[i];
            pieces.erase(pieces.begin() + i);
            track_pieces();
            return;
        }
    }
}

void Player::piece_captured(char SAN_file, int SAN_rank) {
    for (size_t i(0); i < pieces.size(); ++i) {
        if (pieces[i]->get_file() == SAN_file && 
            pieces[i]->get_rank() == SAN_rank) {
            last_captured = *pieces[i];
            delete pieces[i];
            pieces.erase(pieces.begin() + i);
            track_pieces();
            return;
        }
    }
}

void Player::delete_pieces() {
    for (auto& p : pieces) {
        delete p;
    }
    pieces.clear();
}

void Player::reset_en_passant_sqr() {
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        p->clear_en_passant();
    }
}

bool Player::has_en_passant_sqr() {
    for (auto p : pieces) {
        if (p->get_hidden())
            continue;
        if (p->get_has_en_psst()) {
            return true;
        }
    }
    return false;
}

void Player::track_pieces() {

    tracker.king.clear();
    tracker.queen.clear();
    tracker.rook.clear();
    tracker.bishop.clear();
    tracker.knight.clear();
    tracker.pawn.clear();

    for (size_t i(0); i < pieces.size(); ++i) {
        char code(pieces[i]->get_code());
        switch (code) {
            case 'K':
            case 'k':
                tracker.king.push_back(i);
                break;
            case 'Q':
            case 'q':
                tracker.queen.push_back(i);
                break;
            case 'R':
            case 'r':
                tracker.rook.push_back(i);
                break;
            case 'B':
            case 'b':
                tracker.bishop.push_back(i);
                break;
            case 'N':
            case 'n':
                tracker.knight.push_back(i);
                break;
            case 'P':
            case 'p':
                tracker.pawn.push_back(i);
                break;
            default:
                std::wcout << "Unable to track a piece : " << code << "\n";
                break;
        }
    }
}

Piece* Player::find_piece(char code, char file, int rank) {

    switch (code) {
        case 'K':
        case 'k':
            for (auto i : tracker.king) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no king\n";
            break;
        case 'Q':
        case 'q':
            for (auto i : tracker.queen) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no queen : " << file << rank << "\n";
            break;
        case 'R':
        case 'r':
            for (auto i : tracker.rook) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no rook\n";
            break;
        case 'B':
        case 'b':
            for (auto i : tracker.bishop) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no bishop\n";
            break;
        case 'N':
        case 'n':
            for (auto i : tracker.knight) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no knight\n";
            break;
        case 'P':
        case 'p':
            for (auto i : tracker.pawn) {
                if (pieces[i]->get_file() == file && pieces[i]->get_rank() == rank)
                    return pieces[i];
            }
            std::wcout << "no pawn\n";
            break;
        default:
            std::wcout << "Incorrect piece encoding : " << code << "\n";
            break;
    }
    return nullptr;
}

Piece* Player::find_cap_piece(char file, int rank) {
    for (auto& p : pieces) {
        if (p->get_file() == file && p->get_rank() == rank)
            return p;
    }
    return nullptr;
}

/***********
 *  WHITE  *
 ***********/
void White::new_piece(char code, char file, int rank) {
    switch (code) {
        case 'B': pieces.push_back(new Bishop(code, file, rank)); break;
        case 'K': pieces.push_back(new King(code, file, rank)); break;
        case 'N': pieces.push_back(new Knight(code, file, rank)); break;
        case 'P': pieces.push_back(new Pawn(code, file, rank)); break;
        case 'Q': pieces.push_back(new Queen(code, file, rank)); break;
        case 'R': pieces.push_back(new Rook(code, file, rank)); break;
        default: std::wcout << "Couldn't add a new piece to White\n"; break;
    }
    track_pieces();
}

void White::uprise_last_cap() {

    char nc = last_captured.get_code();
    char nf = last_captured.get_file();
    int nr = last_captured.get_rank();

    new_piece(nc, nf, nr);
}

bool White::king_is_last() {
    if (pieces.size() == 1) return true;
    if (pieces.size() > 3) return false;

    for (auto p : pieces) {
        char c(p->get_code());
        if (c == 'R' || c == 'Q' || c == 'P')
            return false;
    }

    int bishops(0);
    for (auto p : pieces) {
        if (p->get_code() == 'B')
            ++bishops;
    }
    if (bishops >= 1 && pieces.size() > 2) return false;

    return true;
}

bool White::can_k_castle() {
    bool no_rook_on_h1(true);
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        if (p->get_code() == 'K' && (p->get_has_moved() || p->get_file() != 'e'))
            return false;
        if (p->get_code() == 'R' && p->get_file() == 'h' && p->get_rank() == 1) {
            no_rook_on_h1 = false;
            if (p->get_has_moved())
                return false;
        }
    }
    if (no_rook_on_h1)
        return false;

    return true;
}

bool White::can_q_castle() {
    bool no_rook_on_a1(true);
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        if (p->get_code() == 'K' && (p->get_has_moved() || p->get_file() != 'e'))
            return false;
        if (p->get_code() == 'R' && p->get_file() == 'a' && p->get_rank() == 1) {
            no_rook_on_a1 = false;
            if (p->get_has_moved())
                return false;
        }
    }
    if (no_rook_on_a1)
        return false;

    return true;
}

void White::castle_king_side(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'K')
            silent ? p->updt_position('g', 1, true) : p->updt_position('g', 1);
        if (p->get_code() == 'R' && p->get_file() == 'h' && p->get_rank() == 1) {
            silent ? p->updt_position('f', 1, true) : p->updt_position('f', 1);
        }
    }
}

void White::castle_queen_side(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'K')
            silent ? p->updt_position('c', 1, true) : p->updt_position('c', 1);
        if (p->get_code() == 'R' && p->get_file() == 'a' && p->get_rank() == 1)
            silent ? p->updt_position('d', 1, true) : p->updt_position('d', 1);
    }
}

void White::undo_k_castle(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'R' && p->get_file() == 'f' && p->get_rank() == 1)
            silent ? p->updt_position('h', 1, true) : p->updt_position('h', 1);
    }
}

void White::undo_q_castle(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'R' && p->get_file() == 'd' && p->get_rank() == 1)
            silent ? p->updt_position('a', 1, true) : p->updt_position('a', 1);
    }
}

/***********
 *  BLACK  *
 ***********/
void Black::new_piece(char code, char file, int rank) {
    switch (code) {
        case 'b': pieces.push_back(new Bishop(code, file, rank)); break;
        case 'k': pieces.push_back(new King(code, file, rank)); break;
        case 'n': pieces.push_back(new Knight(code, file, rank)); break;
        case 'p': pieces.push_back(new Pawn(code, file, rank)); break;
        case 'q': pieces.push_back(new Queen(code, file, rank)); break;
        case 'r': pieces.push_back(new Rook(code, file, rank)); break;
        default: std::wcout << "Couldn't add a new piece to Black\n"; break;
    }
}

void Black::uprise_last_cap() {

    char nc(last_captured.get_code());
    char nf(last_captured.get_file());
    int nr(last_captured.get_rank());

    new_piece(nc, nf, nr);
}

bool Black::king_is_last() {
    if (pieces.size() == 1) return true;
    if (pieces.size() > 3) return false;

    for (auto p : pieces) {
        char c(p->get_code());
        if (c == 'r' || c == 'q' || c == 'p')
            return false;
    }

    int bishops(0);
    for (auto p : pieces) {
        if (p->get_code() == 'b')
            ++bishops;
    }
    if (bishops >= 1 && pieces.size() > 2) return false;

    return true;
}

bool Black::can_k_castle() {
    bool no_rook_on_h8(true);
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        if (p->get_code() == 'k' && (p->get_has_moved() || p->get_file() != 'e'))
            return false;
        if (p->get_code() == 'r' && p->get_file() == 'h' && p->get_rank() == 8) {
            no_rook_on_h8 = false;
            if (p->get_has_moved())
                return false;
        }
    }
    if (no_rook_on_h8)
        return false;

    return true;
}

bool Black::can_q_castle() {
    bool no_rook_on_a8(true);
    for (auto& p : pieces) {
        if (p->get_hidden())
            continue;
        if (p->get_code() == 'k' && (p->get_has_moved() || p->get_file() != 'e')) 
            return false;
        if (p->get_code() == 'r' && p->get_file() == 'a' && p->get_rank() == 8) {
            no_rook_on_a8 = false;
            if (p->get_has_moved())
                return false;
        }
    }
    if (no_rook_on_a8)
        return false;

    return true;
}

void Black::castle_king_side(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'k')
            silent ? p->updt_position('g', 8, true) : p->updt_position('g', 8);
        if (p->get_code() == 'r' && p->get_file() == 'h' && p->get_rank() == 8) {
            silent ? p->updt_position('f', 8, true) : p->updt_position('f', 8);
        }
    }
}

void Black::castle_queen_side(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'k')
            silent ? p->updt_position('c', 8, true) : p->updt_position('c', 8);
        if (p->get_code() == 'r' && p->get_file() == 'a' && p->get_rank() == 8)
            silent ? p->updt_position('d', 8, true) : p->updt_position('d', 8);
    }
}

void Black::undo_k_castle(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'r' && p->get_file() == 'f' && p->get_rank() == 8)
            silent ? p->updt_position('h', 8, true) : p->updt_position('h', 8);
    }
}

void Black::undo_q_castle(bool silent) {
    for (auto& p : pieces) {
        if (p->get_code() == 'r' && p->get_file() == 'd' && p->get_rank() == 8)
            silent ? p->updt_position('a', 8, true) : p->updt_position('a', 8);
    }
}