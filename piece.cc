/*
 *  piece.cc -- chess -- Chess game in the terminal
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
#include "piece.h"
#include "board.h"

/************************
 *  PIECE PARENT CLASS  *
 ************************/
Piece::Piece(char _code, char _file, int _rank)
    : code(_code), file(_file), rank(_rank), hidden(false) {}

Piece::~Piece() {}

bool Piece::is_elligible_for_move(char SAN_piece, char SAN_file, char SAN_rank,
                                  char SAN_spec_file, char SAN_spec_rank) const {

    if (SAN_piece != code && SAN_piece != code + upcase_shift) return false;
    for (auto sq : cov_sqrs) {
        if (sq.file == SAN_file && sq.rank == SAN_rank - '0') {
            if ((SAN_spec_file != blank && SAN_spec_file == file) ||
                (SAN_spec_rank != blank && SAN_spec_rank - '0' == rank))
                return true;
            else if (SAN_spec_file == blank && SAN_spec_rank == blank)
                return true;
        }
    }
    return false;
}

void Piece::updt_position(char SAN_file, char SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank - '0';
}

void Piece::updt_position(char SAN_file, int SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank;
}

bool Piece::attacking_enemy_king() const {
    for (auto sq : cov_sqrs) {
        if (is_enemy_king(code, sq.file, sq.rank)) return true;
    }
    return false;
}

/**********
 *  KING  *
 **********/
King::King(char code, char file, int rank)
    : Piece(code, file, rank), has_moved(false) {
    if (file != 'e')
        has_moved = true;
}

King::~King() {}

void King::updt_cov_sqrs() {
    cov_sqrs.clear();
    for (char c(file-1); c <= file+1; ++c) {
        for (int r(rank-1); r <= rank+1; ++r) {
            if (c >= 'a' &&  c <= 'h' && r >= 1 && r <= board_size &&
                !is_friendly(code, c, r)) {
                cov_sqrs.push_back({c, r});
            }
        }
    }
    if (!has_moved) {
        if (is_empty(char(file-2), rank))
            cov_sqrs.push_back({char(file-2), rank});
        if (is_empty(char(file+2), rank))
            cov_sqrs.push_back({char(file+2), rank});
    }
}

void King::updt_position(char SAN_file, char SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank - '0';
    if (!silent)
        has_moved = true;
}

void King::updt_position(char SAN_file, int SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank;
    if (!silent)
        has_moved = true;
}

/***********
 *  QUEEN  *
 ***********/
Queen::Queen(char code, char file, int rank)
    : Piece(code, file, rank) {}

Queen::~Queen() {}

void Queen::updt_cov_sqrs() {
    cov_sqrs.clear();
    rook_range(cov_sqrs, code, file, rank);
    bishop_range(cov_sqrs, code, file, rank);
}

/**********
 *  ROOK  *
 **********/
Rook::Rook(char code, char file, int rank)
    : Piece(code, file, rank), has_moved(false) {}

Rook::~Rook() {}

void Rook::updt_cov_sqrs() {
    cov_sqrs.clear();
    rook_range(cov_sqrs, code, file, rank);
}

void Rook::updt_position(char SAN_file, char SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank - '0';
    if (!silent)
        has_moved = true;
}

void Rook::updt_position(char SAN_file, int SAN_rank, bool silent) {
    file = SAN_file;
    rank = SAN_rank;
    if (!silent)
        has_moved = true;
}

/************
 *  BISHOP  *
 ************/
Bishop::Bishop(char code, char file, int rank)
    : Piece(code, file, rank) {}

Bishop::~Bishop() {}

void Bishop::updt_cov_sqrs() {
    cov_sqrs.clear();
    bishop_range(cov_sqrs, code, file, rank);
}

/************
 *  KNIGHT  *
 ************/
Knight::Knight(char code, char file, int rank)
    : Piece(code, file, rank) {}

Knight::~Knight() {}

void Knight::updt_cov_sqrs() {
    cov_sqrs.clear();
    if (rank + 2 <= board_size && file + 1 <= 'h' && !is_friendly(code, file+1, rank+2))
        cov_sqrs.push_back({char(file+1), rank+2});
    if (rank + 1 <= board_size && file + 2 <= 'h' && !is_friendly(code, file+2, rank+1))
        cov_sqrs.push_back({char(file+2), rank+1});
    if (rank - 1 >= 1 && file + 2 <= 'h' && !is_friendly(code, file+2, rank-1))
        cov_sqrs.push_back({char(file+2), rank-1});
    if (rank - 2 >= 1 && file + 1 <= 'h' && !is_friendly(code, file+1, rank-2))
        cov_sqrs.push_back({char(file+1), rank-2});
    if (rank - 2 >= 1 && file - 1 >= 'a' && !is_friendly(code, file-1, rank-2))
        cov_sqrs.push_back({char(file-1), rank-2});
    if (rank - 1 >= 1 && file - 2 >= 'a' && !is_friendly(code, file-2, rank-1))
        cov_sqrs.push_back({char(file-2), rank-1});
    if (rank + 1 <= board_size && file - 2 >= 'a' && !is_friendly(code, file-2, rank+1))
        cov_sqrs.push_back({char(file-2), rank+1});
    if (rank + 2 <= board_size && file - 1 >= 'a' && !is_friendly(code, file-1, rank+2))
        cov_sqrs.push_back({char(file-1), rank+2});
}

/**********
 *  PAWN  *
 **********/
Pawn::Pawn(char code, char file, int rank)
    : Piece(code, file, rank), has_moved(false), has_en_passant(false) {}

Pawn::~Pawn() {}

void Pawn::updt_cov_sqrs() {
    cov_sqrs.clear();
    int r(rank);
    bool blocked(false);
    if (code == 'P')
        ++r;
    else
        --r;

    if (r >= 1 && r <= board_size) {
        if (is_empty(file, r))
            cov_sqrs.push_back({file, r});
        else
            blocked = true;
    }

    if (char(file-1) >= 'a') {
        if (is_enemy(code, char(file-1), r))
            cov_sqrs.push_back({char(file-1), r});
        if (is_en_passant_sqr(char(file-1), r))
            cov_sqrs.push_back({char(file-1), r});
    }
    if (char(file+1) <= 'h') {
        if (is_enemy(code, char(file+1), r)) 
            cov_sqrs.push_back({char(file+1), r});
        if (is_en_passant_sqr(char(file+1), r))
            cov_sqrs.push_back({char(file+1), r});
    }
    if (code == 'P') 
        ++r;
    else
        --r;
    if (r >= 1 && r <= board_size) {
        if (code == 'P') {
            if (rank == 2 && !blocked && is_empty(file, r))
                cov_sqrs.push_back({file, r});
        }else if (rank == 7 && !blocked && is_empty(file, r))
            cov_sqrs.push_back({file, r});
    }
}

void Pawn::updt_position(char SAN_file, char SAN_rank, bool silent) {
    if (!has_moved && !silent) {
        if (code == 'P' && SAN_rank - '0' == rank + 2) {
            has_en_passant = true;
            write_en_passant_sqr(file, rank+1);
        }else if (code == 'p' && SAN_rank - '0' == rank - 2) {
            has_en_passant = true;
            write_en_passant_sqr(file, rank-1);
        }
    }
    file = SAN_file;
    rank = SAN_rank - '0';
    if (!silent) {
        has_moved = true;
    }
}

void Pawn::updt_position(char SAN_file, int SAN_rank, bool silent) {
    if (!has_moved && !silent) {
        if (code == 'P' && SAN_rank == rank + 2) {
            has_en_passant = true;
            write_en_passant_sqr(file, rank+1);
        }else if (code == 'p' && SAN_rank == rank - 2) {
            has_en_passant = true;
            write_en_passant_sqr(file, rank-1);
        }
    }
    file = SAN_file;
    rank = SAN_rank;
    if (!silent) {
        has_moved = true;
    }
}

void Pawn::clear_en_passant() {
    has_en_passant = false;
}

void rook_range(std::vector<Square>& covered_sqrs, char code, char file, int rank) {
    for (char c(file+1); c <= 'h'; ++c) {
        if (!is_friendly(code, c, rank)) {
            if (is_enemy(code, c, rank)) {
                covered_sqrs.push_back({c, rank});
                break;
            }else 
                covered_sqrs.push_back({c, rank});
        }else break;
    }
    for (char c(file-1); c >= 'a'; --c) {
        if (!is_friendly(code, c, rank)) {
            if (is_enemy(code, c, rank)) {
                covered_sqrs.push_back({c, rank});
                break;
            }else
                covered_sqrs.push_back({c, rank});
        }else break;
    }
    for (int r(rank+1); r <= board_size; ++r) {
        if (!is_friendly(code, file, r)) {
            if (is_enemy(code, file, r)) {
                covered_sqrs.push_back({file, r});
                break;
            }else
                covered_sqrs.push_back({file, r});
        }else break;
    }
    for (int r(rank-1); r >= 1; --r) {
        if (!is_friendly(code, file, r)) {
            if (is_enemy(code, file, r)) {
                covered_sqrs.push_back({file, r});
                break;
            }else
                covered_sqrs.push_back({file, r});
        }else break;
    }
}

void bishop_range(std::vector<Square>& covered_sqrs, char code, char file, int rank) {
    char c1 = file;
    char c2 = file;
    int r = rank;
    for (int i(1); i <= board_size-rank; ++i) {
        ++c1;
        ++r;
        if (c1 <= 'h' && r <= board_size && !is_friendly(code, c1, r)) {
            if (is_enemy(code, c1, r)) {
                covered_sqrs.push_back({c1, r});
                break;
            }else
                covered_sqrs.push_back({c1, r});
        }else break;
    }
    r = rank;
    for (int i(1); i <= board_size-rank; ++i) {
        --c2;
        ++r;
        if (c2 >= 'a' && r <= board_size && !is_friendly(code, c2, r)) {
            if (is_enemy(code, c2, r)) {
                covered_sqrs.push_back({c2, r});
                break;
            }else
                covered_sqrs.push_back({c2, r});
        }else break;
    }
    c1 = file;
    c2 = file;
    r = rank;
    for (int i(1); i <= rank; ++i) {
        ++c1;
        --r;
        if (c1 <= 'h' && r >= 1 && !is_friendly(code, c1, r)) {
            if (is_enemy(code, c1, r)) {
                covered_sqrs.push_back({c1, r});
                break;
            }else
                covered_sqrs.push_back({c1, r});
        }else break;
    }
    r = rank;
    for (int i(1); i <= rank; ++i) {
        --c2;
        --r;
        if (c2 >= 'a' && r >= 1 && !is_friendly(code, c2, r)) {
            if (is_enemy(code, c2, r)) {
                covered_sqrs.push_back({c2, r});
                break;
            }else
                covered_sqrs.push_back({c2, r});
        }else break;
    }
}