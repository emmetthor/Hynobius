#pragma once

#include "Type.h"
#include "board/Board.h"
#include "board/Piece.h"

#include <vector>

Piece playerPieceCharToPiece(Player player, char piece);

std::vector<Position> findValidPieceWithColume(Board &board, Piece p, int col);

std::vector<Position> findValidPieceWithRow(Board &board, Piece p, int row);
