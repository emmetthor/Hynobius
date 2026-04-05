#pragma once

#include "Type.h"
#include "move/Move.h"

struct ScoreMove
{
    BitMove move;
    int score;
};

struct advanceMoves
{
    BitMove TTMove, killer1, killer2;
};

void sortMove(const Board& board, Move* moves, int nMoves, advanceMoves& advMove);
