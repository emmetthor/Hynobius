#pragma once

#include "Type.h"
#include "board/Board.h"
#include "move/Move.h"

int generateAllLegalMoves(const Board& board, Move* buffer);

int generateLegalCaptureMoves(const Board& board, Move* buffer);

int generateAllLegalMoves(const Board& board, BitMove* buffer);

int generateLegalCaptureMoves(const Board& board, BitMove* buffer);