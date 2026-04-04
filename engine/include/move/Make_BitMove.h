#pragma once
#include "board/Board.h"
#include "move/Move.h"
#include <board/Piece.h>
#include <cstdint>

// intergret move infos
// placedPiece: The piece that actually placed at Position to
struct MoveState
{
    Position from, to;

    Piece movePiece, capturedPiece, placedPiece;

    bool isCastle, isPromotion;

    MoveState(const Board& board, const BitMove move)
    {
        from = squareToPosition(getFromSquare(move));
        to = squareToPosition(getToSquare(move));

        movePiece = board.at(from);
        capturedPiece = board.at(to);

        isCastle = getCastle(move);
        isPromotion = getPromotion(move);

        if (isPromotion)
        {
            placedPiece = getPromotePiece(move);
        }
        else
        {
            placedPiece = movePiece;
        }
    }
};

struct UndoState
{
    Piece capturedPiece;
    Piece movePiece;
    Piece placedPiece;
    Position from;
    Position to;
    bool isCastle;
    bool isPromotion;

    int castleRights;
    int materialScore;
    int PSTScore;
    uint64_t zobristKey;

    Player player;

    inline void make(const Board& board, const MoveState& state)
    {
        capturedPiece = state.capturedPiece;
        movePiece = state.movePiece;
        placedPiece = state.placedPiece;
        from = state.from;
        to = state.to;
        isCastle = state.isCastle;
        isPromotion = state.isPromotion;
        castleRights = board.castleRights;
        materialScore = board.materialScore;
        PSTScore = board.PSTScore;
        zobristKey = board.zobristKey;
        player = board.player;
    }
};

void doBitMove(Board& board, const BitMove move, UndoState& undo);

void undoBitMove(Board& board, const BitMove move, const UndoState& undo);