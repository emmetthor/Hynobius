#pragma once

#include "Evaluate_Weight.h"
#include "Type.h"
#include "board/Board.h"

class Evaluate
{
public:
    int evaluateBoard(const Board& board, EVALUATE_MODE m) const;
    Evaluate_Weights evalWeight;
};

int boardEvaluate(const Board& board, EVALUATE_MODE m);