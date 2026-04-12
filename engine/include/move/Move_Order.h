#pragma once

#include "Type.h"
#include "board/Board.h"

struct ScoreMove
{
    BitMove move;
    int score;
};

struct advanceMoves
{
    BitMove PVMove;
    BitMove TTMove;
};

void sortMove(const Board& board, BitMove* moves, int nMoves, const advanceMoves& adv);
