#include "move/Make_BitMove.h"
#include "move/Move.h"
#pragma GCC optimize("O3,unroll-loops")

#include "evaluate/Material_Point.h"
#include "move/Make_BitMove.h"
#include "move/Move_Order.h"

#include <algorithm>

const int TT_SCORE = 600000;
const int PVMOVE_SCORE = 500000;
const int PROMOTION_SCORE = 400000;
const int CAPTURE_SCORE = 300000;
const int KILLER_1_SCORE = 200000;
const int KILLER_2_SCORE = 100000;

int evaluateMoveScore(const Board& board, const BitMove move, const advanceMoves& adv)
{
    MoveState state(board, move);

    int score = 0;

    if (adv.PVMove == move)
        score += PVMOVE_SCORE;

    if (adv.TTMove == move)
        score += TT_SCORE;

    if (adv.killerMove1 == move)
        score += KILLER_1_SCORE;

    if (adv.killerMove2 == move)
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

    return score;
}

void sortMove(const Board& board, BitMove* moves, int nMoves, const advanceMoves& adv)
{
    ScoreMove tmp[256];

    for (int i = 0; i < nMoves; i++)
    {
        tmp[i].move = moves[i];
        tmp[i].score = evaluateMoveScore(board, moves[i], adv);
    }

    std::sort(tmp,
              tmp + nMoves,
              [](const ScoreMove& a, const ScoreMove& b) { return a.score > b.score; });

    for (int i = 0; i < nMoves; i++)
    {
        moves[i] = tmp[i].move;
    }
}