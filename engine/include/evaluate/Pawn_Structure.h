#pragma once

#include "board/Board.h"

int evaluatePawnStructure(const Board& board,
                          const int doublePawnWeight,
                          const int isolatedPawnWeight,
                          const int passedPawnWeight,
                          const int* passedPawnRankWeight);
