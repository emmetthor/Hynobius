#pragma once

#include "debug.h"

#include <cctype>

// Define `Piece`.
enum class Piece
{
    EMPTY = 0,
    WPAWN,
    WKNIGHT,
    WBISHOP,
    WROOK,
    WQUEEN,
    WKING,
    BPAWN,
    BKNIGHT,
    BBISHOP,
    BROOK,
    BQUEEN,
    BKING,
    PIECE_COUNT
};

// Turn `Piece` into char.
inline constexpr char PIECE_TO_CHAR[]{
    '.', 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

// Turn [`Player`][`charToPieceIndex`] into `Piece`.
inline constexpr Piece MAKE_PIECE_MAP[2][6]{
    {Piece::WPAWN, Piece::WKNIGHT, Piece::WBISHOP, Piece::WROOK, Piece::WQUEEN, Piece::WKING},
    {Piece::BPAWN, Piece::BKNIGHT, Piece::BBISHOP, Piece::BROOK, Piece::BQUEEN, Piece::BKING}};

// Turn char into index to support `MAKE_PIECE_MAP`.
// - Lower and upper letter are supported.
inline int charToPieceIndex(char c)
{
    switch (std::toupper(c))
    {
        case 'P':
            return 0;
        case 'N':
            return 1;
        case 'B':
            return 2;
        case 'R':
            return 3;
        case 'Q':
            return 4;
        case 'K':
            return 5;
        default:
            ENGINE_FATAL(DebugCategory::PIECE, "invalid input: ", c);
    }
}

// Turn `Piece` to `int` safely.
inline int pieceToIndex(Piece p)
{
    return static_cast<int>(p);
}

// Turn `Piece` to `char`.
inline char pieceToChar(Piece p)
{
    int pIndex = pieceToIndex(p);

    if (!(0 <= pIndex && pIndex <= 12))
    {
        ENGINE_FATAL(DebugCategory::PIECE, "invalid piece index: ", pIndex);
    }

    return PIECE_TO_CHAR[pieceToIndex(p)];
}

// Check whether a piece is white.
inline bool isWhite(Piece p)
{
    if (p == Piece::EMPTY)
    {
        ENGINE_FATAL(DebugCategory::PIECE, "empty piece does not have a color");
    }

    int wp = pieceToIndex(Piece::WPAWN), wk = pieceToIndex(Piece::WKING),
        currentPiece = pieceToIndex(p);
    if (wp <= currentPiece && currentPiece <= wk)
        return true;
    else
        return false;
}

// Check whether a piece is black.
inline bool isBlack(Piece p)
{
    if (p == Piece::EMPTY)
    {
        ENGINE_FATAL(DebugCategory::PIECE, "empty piece does not have a color");
    }

    int bp = pieceToIndex(Piece::BPAWN), bk = pieceToIndex(Piece::BKING),
        currentPiece = pieceToIndex(p);
    if (bp <= currentPiece && currentPiece <= bk)
        return true;
    else
        return false;
}

// Check whether two pieces share the same color.
inline bool isSameColor(Piece p1, Piece p2)
{
    if (p1 == Piece::EMPTY)
    {
        ENGINE_FATAL(DebugCategory::PIECE, "empty piece does not have a color");
    }
    if (p2 == Piece::EMPTY)
    {
        ENGINE_FATAL(DebugCategory::PIECE, "empty piece does not have a color");
    }

    return isWhite(p1) == isWhite(p2);
}

inline bool isValidPieceIndex(int index)
{
    return (1 <= index && index <= 12 ? true : false);
}