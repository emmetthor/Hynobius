#pragma once

#include "Type.h"
#include "board/Board.h"

struct Move;

int countPawnAttacks(
    const Board &board,
    Position pos,
    const Player player
);
int countKnightAttacks(
    const Board &board,
    Position pos,
    const Player player
);

int countSquareAttacks(
    const Board &board,
    Position pos,
    const Player player
);
bool isSquareAttacked(
    const Board &board,
    Position pos,
    const Player player
);

int getAttackers(
    const Board &board,
    Position pos,
    Player player,
    Position* buffer
);