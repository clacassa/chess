#ifndef GAME_H
#define GAME_H

#include <string>
#include "player.h"

const std::wstring king_castle(L"O-O");
const std::wstring queen_castle(L"O-O-O");
const std::string ini_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

class Game {
public:
    Game();
    virtual ~Game();
    void game_flow(bool black, bool pvp, bool cvc);
    void prompt_move();
    void print_position();
    void updt_board();

    bool parse_fen(std::string fen);
    void fen_verify_checks();

    bool parse_cmd(std::wstring cmd);

    bool is_SAN_valid(std::wstring SAN);
    bool decode_SAN(char c, char prev, bool done, int i, int len);

    int process_move();
    int is_resign();
    void resign();

    bool computer_play();

    void test_gen_moves(int max_depth=4);
private:
    White white;
    Black black;
    bool w_turn, capture, promote, q_castle, k_castle, check, SAN_chk,
        SAN_cap, w_en_psst, b_en_psst;
    char SAN_piece, SAN_file, SAN_rank, SAN_spec_file, SAN_spec_rank, 
        prom_piece;
    Square start, target, last_move;
    Piece* attacker;
    void piece_from_fen(char code, char file, int rank);
    bool is_move_legal(Piece* p, char trgt_file, int trgt_rank, bool w_ply);
    bool handle_check(Piece* piece, char trgt_file, int trgt_rank, bool capt, bool w_p);
    bool process_move(Piece* p, char trgt_file, int trgt_rank, bool w_ply);
    bool process_castling();
    bool is_checkmate();
    bool is_draw();
    int compute_moves(int depth=1, bool w_turn=true);
};

#endif