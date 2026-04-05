#pragma once

#include "Type.h"
#include "board/Board.h"
#include "board/Piece.h"
#include "move/Move.h"
#include "pgn/Pgn_Transformer.h"
#include <ostream>
#include <vector>

inline std::ostream& operator<<(std::ostream& os, const Position& p)
{
    return os << "(" << p.row << "," << p.col << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Board& p)
{
    for (int r = 0; r < 9; r++)
    {
        for (int c = -1; c < 8; c++)
        {
            if (r == 8 && c == -1)
            {
                os << "  ";
                continue;
            }

            if (c == -1)
            {
                os << rowToPgn[r] << ' ';
                continue;
            }

            if (r == 8)
            {
                os << colToPgn[c] << ' ';
                continue;
            }

            os << pieceToChar(p.board[r][c]) << " \n"[c == 8 - 1];
        }
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Move& move)
{
    if (move.castle == SHORT_CASTLE)
        os << "O-O";
    else if (move.castle == LONG_CASTLE)
        os << "O-O-O";
    else
        os << pieceToChar(move.movePiece) << positionToPgn(move.from) << positionToPgn(move.to);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const BitMove& move)
{
    Position from = squareToPosition(getFromSquare(move));
    Position to = squareToPosition(getToSquare(move));
    os << positionToPgn(from) << positionToPgn(to);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Player player)
{
    return os << playerToIndex(player);
}

inline std::ostream& operator<<(std::ostream& os, const Piece p)
{
    return os << pieceToChar(p);
}

template <typename T> struct Table
{
    std::vector<std::string> titles;  // 標題列
    std::vector<std::vector<T>> rows; // 內容列

    // 設定標題
    void setTitles(const std::vector<std::string>& t)
    {
        titles = t;
    }

    // 新增一行
    void addRow(const std::vector<T>& row)
    {
        rows.push_back(row);
    }

    // 設定欄寬 (預設 12)
    size_t colWidth = 12;
};

template <typename T> std::ostream& operator<<(std::ostream& os, const Table<T>& table)
{
    os << std::left;

    // 標題列
    for (const auto& title : table.titles)
        os << std::setw(table.colWidth) << title;
    os << "\n";

    // 分隔線
    os << std::string(table.colWidth * table.titles.size(), '-') << "\n";

    // 內容列
    for (const auto& row : table.rows)
    {
        for (const auto& cell : row)
            os << std::setw(table.colWidth) << cell;
        os << "\n";
    }

    return os;
}
