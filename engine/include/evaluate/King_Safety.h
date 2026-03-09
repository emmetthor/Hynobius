#pragma once

#include "Type.h"
#include "board/Board.h"
#include "board/Attack.h"

int evaluateKingSafety(const Board &board, Player player, int weight);