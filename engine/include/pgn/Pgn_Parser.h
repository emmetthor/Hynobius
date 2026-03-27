#pragma once

#include "board/Piece.h"
#include "move/Move.h"

#include <string>
#include <vector>

class PGN
{
public:
    void cinPgnToSan();
    void SantoMove();
    std::vector<Move> getMoves();
    int getMovesCount();
    Move SantoMoveSingle(Board& board, std::string strSan, Player player);

private:
    std::string whiteName, blackName, result, whiteElo, blackElo;

    std::vector<std::string> san_moves;

    std::vector<Move> moves;
};

struct SAN
{
    Piece piece, promorePiece;
    bool isCapture = false;
    bool isPromote = false;
    Position toPos, fromPos;
};