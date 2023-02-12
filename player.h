#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>
#include <string>
#include "piece.h"
#include "common.h"

class Player {
public:
    Player() { reset_en_passant_sqr(); }
    virtual ~Player() { delete_pieces(); }

    Piece* unique_piece_for_move(char SAN_piece, char SAN_col, char SAN_rank, 
                                 char SAN_spec_file, char SAN_spec_rank);
    Piece* attacker();

    void write_pieces_on_board();
    // void new_piece(char code, char file, char rank);
    void reveal_piece(char file, int rank);
    void hide_piece(char file, int rank);
    void piece_captured(char file, char rank);
    void piece_captured(char file, int rank);
    void delete_pieces();

    void reset_en_passant_sqr();
    bool has_en_passant_sqr();

    void track_pieces();
    Piece* find_piece(char code, char file, int rank);
    Piece* find_cap_piece(char file, int rank);

    virtual void new_piece(char code, char file, int rank) = 0;

    virtual void uprise_last_cap() = 0;
    virtual bool king_is_last() = 0;

    virtual bool can_k_castle(bool chk_empty_sqrs=true) = 0;
    virtual bool can_q_castle(bool chk_empty_sqrs=true) = 0;
    virtual void castle_king_side(bool silent=false) = 0;
    virtual void castle_queen_side(bool silent=false) = 0;
    virtual void undo_k_castle(bool silent=false) = 0;
    virtual void undo_q_castle(bool silent=false) = 0;

    Piece* get_piece(size_t i) { return pieces[i]; }
    Army* get_pieces() { return &pieces; }
    size_t get_nb_pieces() const { return pieces.size(); }

protected:

    struct PieceTracker {
        std::vector<size_t> king;
        std::vector<size_t> queen;
        std::vector<size_t> rook;
        std::vector<size_t> bishop;
        std::vector<size_t> knight;
        std::vector<size_t> pawn;
    };

    PieceTracker tracker;
    Army pieces, elligible_pieces;
    Piece last_captured;
};

class White : public Player {
public:
    White() {}
    virtual ~White() {}
    void new_piece(char code, char file, int rank) override;
    void uprise_last_cap() override;
    bool king_is_last() override;
    bool can_k_castle(bool chk_empty_sqrs=true) override;
    bool can_q_castle(bool chk_empty_sqrs=true) override;
    void castle_king_side(bool silent=false) override;
    void castle_queen_side(bool silent=false) override;
    void undo_k_castle(bool silent=false) override;
    void undo_q_castle(bool silent=false) override;
private:
};

class Black : public Player {
public:
    Black() {}
    virtual ~Black() {}
    void new_piece(char code, char file, int rank) override;
    void uprise_last_cap() override;
    bool king_is_last() override;
    bool can_k_castle(bool chk_empty_sqrs=true) override;
    bool can_q_castle(bool chk_empty_sqrs=true) override;
    void castle_king_side(bool silent=false) override;
    void castle_queen_side(bool silent=false) override;
    void undo_k_castle(bool silent=false) override;
    void undo_q_castle(bool silent=false) override;
private:
};

#endif