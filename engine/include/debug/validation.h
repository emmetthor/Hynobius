#pragma once

#include "board/Board.h"
#include "debug/log.h"

inline void checkBoardState(const Board& board)
{
#ifdef ENGINE_VALIDATE

    // king must exist
    int whiteKing = 0;
    int blackKing = 0;

    // recompute material
    int recomputedMaterial = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board.board[r][c];

            if (p == Piece::EMPTY)
                continue;

            // king 檢查
            if (p == Piece::WHITE_KING)
                whiteKing++;
            if (p == Piece::BLACK_KING)
                blackKing++;

            // material
            recomputedMaterial += pieceValue(p) * (isWhite(p) ? 1 : -1);
        }
    }

    // king invariant
    if (whiteKing != 1 || blackKing != 1)
    {
        ENGINE_FATAL("BOARD", "invalid king count ", "white=", whiteKing, " black=", blackKing);
    }

    // material invariant
    if (recomputedMaterial != board.materialScore)
    {
        ENGINE_FATAL("EVAL",
                     "material mismatch ",
                     "cached=",
                     board.materialScore,
                     " recomputed=",
                     recomputedMaterial);
    }

#endif
}