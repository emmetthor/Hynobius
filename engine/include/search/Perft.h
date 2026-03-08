#pragma once

#include "board/Board.h"
#include "move/Make_Move.h"
#include "move/Generate_Move.h"
#include "Structure_IO.h"

#include <iostream>

int perft(Board &board, int depth) {
    if (depth <= 0) return 1;
    int nodes = 0;

    Move moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    for (int i = 0; i < nMoves; i++) {
        Move move = moves[i];
        makeMove(board, move);
        nodes += perft(board, depth - 1);
        undoMove(board, move);
    }

    return nodes;
}