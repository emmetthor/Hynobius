#pragma once

#include "../engine/include/move/Move.h"
#include "../engine/include/board/Piece.h"
#include "../engine/include/board/Board.h"
#include "../engine/include/debug.h"

bool bruteMoveTest() {
    for (Square from = 0; from < 64; from++) {
        for (Square to = 0; to < 64; to++) {
            for (int pieceIndex = 1; pieceIndex <= 12; pieceIndex++) {
                for (int flag = 0; flag < (1 << 4); flag++) {
                    bool capture = flag & 1;
                    bool castle = flag & 2;
                    bool enPassant = flag & 4;
                    bool promote = flag & 8;

                    BitMove m = makeBitMove(
                        from,
                        to,
                        static_cast<Piece>(pieceIndex),
                        capture,
                        castle,
                        enPassant,
                        promote
                    );

                    assert(getFromSquare(m) == from);
                    assert(getToSquare(m) == to);
                    assert(getPromotePiece(m) == static_cast<Piece>(pieceIndex));
                    assert(getCapture(m) == capture);
                    assert(getCastle(m) == castle);
                    assert(getEnPassant(m) == enPassant);
                    assert(getPromotion(m) == promote);
                }
            }
        }
    }

    return true;
}