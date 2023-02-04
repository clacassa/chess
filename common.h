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

const std::wstring ILLEGAL(L"\x1b[31mIllegal move!\x1b[0m");
const std::wstring AMBIGUEOUS(L"Ambigueous move!\n");
const std::wstring CHECKMATE(L"Checkmate!");
const std::wstring STALEMATE(L"Stalemate!");
const std::wstring NO_WIN(L"Neither side can win!");

const std::wstring WHITE_WINS(L"1-0\n");
const std::wstring BLACK_WINS(L"0-1\n");
const std::wstring DRAW(L"\u00BD-\u00BD\n");
const std::wstring WHITE_RESIGNS(L"White resigns");
const std::wstring BLACK_RESIGNS(L"Black resigns");

const std::wstring NOT_A_CHECK(L"This is not a check ;)\n");
const std::wstring NOT_A_CAPTURE(L"There is no enemy on ");

const std::wstring INVALID_SAN(L"Invalid SAN format\n");
const std::wstring FEN_ERROR(L"Error parsing FEN file: bad FEN format\n");
const std::wstring FILE_ERROR(L"Failed opening the file\n");
const std::wstring FEN_FILE_BAD_EXTENSION(L"The file is not an FEN file\n");

constexpr size_t SAN_min_char(2);
constexpr size_t SAN_max_char(7);
constexpr int upcase_shift(-32);
constexpr char blank('?');

#endif