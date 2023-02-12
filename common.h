#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <array>

#define BULK_COUNTING
// #define DIVIDE

constexpr int board_size(8);
typedef std::array<std::array<char, board_size>, board_size> Board;

struct Square {
    char file;
    int rank;
};

struct Move {
    Move(char _piece, Square _start, Square _target, char _prom) 
    :   piece(_piece), start(_start), target(_target), prom(_prom) {}

    char piece;
    Square start;
    Square target;
    char prom;
};

constexpr size_t SAN_min_char(2);
constexpr size_t SAN_max_char(7);
constexpr int upcase_shift(-32);
constexpr char blank('?');

#endif