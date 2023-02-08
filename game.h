#ifndef GAME_H
#define GAME_H

#include <string>
#include "player.h"

const std::wstring king_castle(L"O-O");
const std::wstring queen_castle(L"O-O-O");
const std::string ini_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

struct TestResult {
    int n_cptrs;
    int n_cstls;
    int n_proms;
    int n_chks;
    int n_chkmates;
};

class Game {
public:
    Game();
    virtual ~Game();
    void game_flow(bool black, bool pvp, bool cvc);
    void prompt_move();
    void print_position(bool w_ply);
    void updt_board();

    bool parse_fen(std::string fen);
    void fen_verify_checks();

    bool parse_cmd(std::wstring cmd);

    bool is_SAN_valid(std::wstring SAN);
    bool decode_SAN(char c, char prev, bool done, int i, int len);

    int is_resign();
    void resign();

    bool computer_play();

    void test_gen_moves(int max_depth=6);
private:
    White white;
    Black black;
    Square start, target;
    Move current_move, last_move;
    Piece* attacker;
    TestResult t_result;

    std::wstring SAN;
    char SAN_piece, SAN_file, SAN_rank, SAN_spec_file, SAN_spec_rank, SAN_prom_pc;
    bool SAN_cap, SAN_chk, SAN_promote;

    bool w_turn, capture, q_castle, k_castle, check, checkmate, w_en_psst, b_en_psst;

    int nb_move;
    
    void piece_from_fen(char code, char file, int rank);

    bool is_move_legal(Piece* p, char trgt_file, int trgt_rank, bool w_ply);
    bool handle_check(Piece* piece, char trgt_file, int trgt_rank, bool capt, bool w_p);
    bool process_move(Piece* p, char trgt_file, int trgt_rank, bool w_ply, 
                                                               bool test=false);
    bool is_checkmate(bool w_ply);
    bool is_draw(bool w_ply);

    void reset_san_variables();

    int compute_moves(int depth=1, bool w_turn=true);
};

#endif