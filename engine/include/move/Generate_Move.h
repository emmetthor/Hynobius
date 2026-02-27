#pragma once

#include "board/Board.h"
#include "board/Piece.h"
#include "move/Move.h"
#include "move/Generate_Position.h"

#include <vector>

int generateAllLegalMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
);

int generateLegalCaptureMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
);