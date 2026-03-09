# pragma once

#include "Type.h"
#include "board/Board.h"

int evaluateKnightMobility (
    const Board &board,
    Player player,
    int knightMobilityWeight
);
int evaluateBishopMobility (
    const Board &board,
    Player player,
    int bishopMobilityWeight
);
int evaluateRookMobility (
    const Board &board,
    Player player,
    int rookMobilityWeight
);