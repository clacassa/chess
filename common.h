#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <array>

constexpr int board_size(8);
typedef std::array<std::array<char, board_size>, board_size> Board;

struct Square {
    char file;
    int rank;
};

struct Move {
    Move(int _num, char _piece, Square _start, Square _target) {
        num = _num;
        piece = _piece;
        start = _start;
        target = _target;
    }
    int num;
    char piece;
    Square start;
    Square target;
};

constexpr size_t SAN_min_char(2);
constexpr size_t SAN_max_char(7);
constexpr int upcase_shift(-32);
constexpr char blank('?');

#endif