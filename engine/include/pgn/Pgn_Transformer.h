#pragma once

#include "board/Board.h"

#include <string>

extern const char colToPgn[8];
extern const char rowToPgn[8];

Position pgnToPosition(std::string s);

std::string positionToPgn(Position pos);