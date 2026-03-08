#pragma once

#include "board/Board.h"
#include "move/Make_Move.h"
#include "move/Generate_Move.h"
#include "Structure_IO.h"
#include "search/Zobrist.h"

#include <iostream>

int boardConsistency(Board &board, int depth) {
    if (depth <= 0) return 1;
    
    if (board.zobristKey != computeZobrist(board)) {
        ENGINE_FATAL(DebugCategory::BOARD, "different zobrist");
    }

    for (int pIndex = 1; pIndex <= 12; pIndex++) {
        Piece p = static_cast<Piece>(pIndex);
        for (int i = 0; i < board.getPieceCount(p); i++) {
            if (board.at(board.getPiecePos(p)[i]) != p) {
                ENGINE_FATAL(DebugCategory::BOARD, "different piece position");
            }
        }        
    }

    int node = 0;

    Move moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    for (int i = 0; i < nMoves; i++) {
        Move move = moves[i];
        makeMove(board, move);
        node += boardConsistency(board, depth - 1);
        undoMove(board, move);
    }

    return node;
}