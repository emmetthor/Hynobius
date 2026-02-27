#pragma once

#include "Type.h"
#include "board/Board.h"

enum Castle
{
    NOT,
    SHORT_CASTLE,
    LONG_CASTLE
};

/*
bit  0  1  2 | 3  4  5 | Position from
bit  6  7  8 | 9 10 11 | Position to
bit 12 13 14 15          Promote Piece
bit 16                   is capture
bit 17                   is castle
bit 18                   is en passant
bit 19                   is promotion
WARN 其實可以直接從 Promote Piece 看出是否是 promote
*/

constexpr BitMove FROM_MASK = 0b111111;
constexpr BitMove TO_MASK = 0b111111 << 6;
constexpr BitMove PROMOTE_PIECE_MASK = 0b1111 << 12;
constexpr BitMove CAPTURE_MASK = 1u << 16;
constexpr BitMove CASTLE_MASK = 1u << 17;
constexpr BitMove EN_PASSENT_MASK = 1u << 18;
constexpr BitMove PROMOTION_MASK = 1u << 19;

inline Square getFromSquare(const BitMove move)
{
    Square res = static_cast<Square>(move & FROM_MASK);

    if (!isValidSquare(res))
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid from square: ", res);
    }

    return res;
}

inline Square getToSquare(const BitMove move)
{
    Square res = static_cast<Square>((move & TO_MASK) >> 6);

    if (!isValidSquare(res))
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid to square: ", res);
    }

    return res;
}

inline Piece getPromotePiece(const BitMove move)
{
    int pieceIndex = static_cast<int>((move & PROMOTE_PIECE_MASK) >> 12);

    if (!isValidPieceIndex(pieceIndex))
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid piece index: ", pieceIndex);
    }

    return static_cast<Piece>(pieceIndex);
}

inline bool getCapture(const BitMove move)
{
    return static_cast<bool>(move & CAPTURE_MASK);
}

inline bool getCastle(const BitMove move)
{
    return static_cast<bool>(move & CASTLE_MASK);
}

inline bool getEnPassant(const BitMove move)
{
    return static_cast<bool>(move & EN_PASSENT_MASK);
}

inline bool getPromotion(const BitMove move)
{
    return static_cast<bool>(move & PROMOTION_MASK);
}

inline BitMove makeBitMove(const Square from,
                           const Square to,
                           const Piece promotePiece,
                           const bool isCapture,
                           const bool isCastle,
                           const bool isEnPassant,
                           const bool isPromotion)
{
    BitMove res = 0;

    res |= from;
    res |= (to << 6);
    res |= (pieceToIndex(promotePiece) << 12);
    res |= (static_cast<int>(isCapture) << 16);
    res |= (static_cast<int>(isCastle) << 17);
    res |= (static_cast<int>(isEnPassant) << 18);
    res |= (static_cast<int>(isPromotion) << 19);

    return res;
}

struct Move
{
    Player player;

    Position from;
    Position to;
    Piece movePiece, capturePiece = Piece::EMPTY;

    bool isPromotion = false;
    Piece promotionPiece = Piece::EMPTY;

    Castle castle = NOT;

    bool isEnPassant = false;

    int prevCastleRights;
    int prevMateralPoints;
    int prevPST;
    uint64_t prevZobrist;

    bool operator==(const Move& other) const
    {
        return player == other.player && from == other.from && to == other.to &&
               movePiece == other.movePiece && capturePiece == other.capturePiece &&
               isPromotion == other.isPromotion && promotionPiece == other.promotionPiece &&
               castle == other.castle && isEnPassant == other.isEnPassant;
    }
};

extern Move inValidMove;

struct CastleMove
{
    Position kingFrom, kingTo;
    Position rookFrom, rookTo;
    Piece kingPiece;
    Piece rookPiece;
};

void printMove(const Move& move);

void moveDebugPrint(const Move& move);

bool isMoveLegal(const Board& board, const Move& move);

bool isCastleLegal(const Board& board, const Move& move);

CastleMove getCastleMove(Move& move);