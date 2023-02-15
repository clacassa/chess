#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <string>
#include "common.h"

class Piece {
public:
    Piece(char code = '.', char file = 'a', int rank = 1);
    virtual ~Piece();

    virtual void updt_cov_sqrs() {}
    bool is_elligible_for_move(char SAN_piece, char SAN_file, char SAN_rank,
                               char SAN_spec_file, char SAN_spec_rank) const ;
    virtual void updt_position(char SAN_file, char SAN_rank, bool silent=false);
    virtual void updt_position(char SAN_file, int SAN_rank, bool silent=false);
    bool attacking_enemy_king() const ;

    /* Only makes sense for the Pawn derived class */
    virtual void clear_en_passant() {}
    virtual bool get_has_en_psst() const { return false; }

    void hide() { hidden = true; }
    void reveal() { hidden = false; }
    virtual void set_has_moved(bool _has_moved) {}

    char get_code() const { return code; }
    char get_file() const { return file; }
    int get_rank() const { return rank; }
    bool get_hidden() const { return hidden; }
    virtual bool get_has_moved() const { return false; }
    std::vector<Square> get_cov_sqrs() const { return cov_sqrs; }
    virtual std::vector<Square> get_atck_sqrs() const { return cov_sqrs; }
protected:
    char code;
    char file;
    int rank;
    bool hidden;
    std::vector<Square> cov_sqrs;
};

typedef std::vector<Piece*> Army;

class King : public Piece {
public:
    King(char code, char file, int rank);
    virtual ~King();
    void updt_cov_sqrs() override;
    void updt_position(char SAN_file, char SAN_rank, bool silent=false) override;
    void updt_position(char SAN_file, int SAN_rank, bool silent=false) override;
    void set_has_moved(bool _has_moved) override { has_moved = _has_moved; }
    bool get_has_moved() const override { return has_moved; }
private:
    bool has_moved;    
};

class Queen : public Piece {
public:
    Queen(char code, char file, int rank);
    virtual ~Queen();
    void updt_cov_sqrs() override;
private:
};

class Rook : public Piece {
public:
    Rook(char code, char file, int rank);
    virtual ~Rook();
    void updt_cov_sqrs() override;
    void updt_position(char SAN_file, char SAN_rank, bool silent=false) override;
    void updt_position(char SAN_file, int SAN_rank, bool silent=false) override;
    void set_has_moved(bool _has_moved) override { has_moved = _has_moved; }
    bool get_has_moved() const override { return has_moved; }
private:
    bool has_moved;
};

class Bishop : public Piece {
public:
    Bishop(char code, char file, int rank);
    virtual ~Bishop();
    void updt_cov_sqrs() override;
private:
};

class Knight : public Piece {
public:
    Knight(char code, char file, int rank);
    virtual ~Knight();
    void updt_cov_sqrs() override;
private:
};

class Pawn : public Piece {
public:
    Pawn(char code, char file, int rank);
    virtual ~Pawn();
    void updt_cov_sqrs() override;
    void updt_position(char SAN_file, char SAN_rank, bool silent=false) override;
    void updt_position(char SAN_file, int SAN_rank, bool silent=false) override;
    void clear_en_passant() override;
    bool get_has_en_psst() const override { return has_en_passant; }
    std::vector<Square> get_atck_sqrs() const override;
private:
    bool has_moved, has_en_passant;
    Square en_passant_sqr;
};

void rook_range(std::vector<Square>&, char code, char file, int rank);
void bishop_range(std::vector<Square>&, char code, char file, int rank);

#endif