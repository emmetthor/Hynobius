#include "move/Make_BitMove.h"
#include "move/Move.h"
#pragma GCC optimize("O3,unroll-loops")

#include "evaluate/Material_Point.h"
#include "evaluate/SEE.h"
#include "move/Make_BitMove.h"
#include "move/Move_Order.h"

#include <algorithm>

const int TT_SCORE = 1000000;
const int CAPTURE_SCORE = 500000;
const int KILLER_1_SCORE = 400000;
const int KILLER_2_SCORE = 300000;
const int PROMOTION_SCORE = 750000;

int evaluateMoveScore(const Board& board, const BitMove move, advanceMoves& advMove)
{
    MoveState state(board, move);

    int score = 0;

    if (move == advMove.TTMove)
        score += TT_SCORE;
    if (move == advMove.killer1)
        score += KILLER_1_SCORE;
    if (move == advMove.killer2)
        score += KILLER_2_SCORE;

    if (state.isPromotion)
    {
        // 最先看
        score += PROMOTION_SCORE;
        score += pieceValue(getPromotePiece(move));
    }

    if (state.capturedPiece != Piece::EMPTY)
    {
        score += CAPTURE_SCORE;
        score += pieceValue(state.capturedPiece) * 100 - pieceValue(state.capturedPiece);
    }

    score += SEE(board, state.to, state.movePiece, board.player);

    return score;
}

void sortMove(const Board& board, BitMove* moves, int nMoves, advanceMoves& advMove)
{
    ScoreMove tmp[256];

    for (int i = 0; i < nMoves; i++)
    {
        tmp[i].move = moves[i];
        tmp[i].score = evaluateMoveScore(board, moves[i], advMove);
    }

    std::sort(tmp,
              tmp + nMoves,
              [](const ScoreMove& a, const ScoreMove& b) { return a.score > b.score; });

    for (int i = 0; i < nMoves; i++)
    {
        moves[i] = tmp[i].move;
    }
}