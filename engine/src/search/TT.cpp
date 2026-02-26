#include "search/TT.h"
#include "search/Search.h"

#include <cstdint>
#include <assert.h>

TTEntry TT[TT_SIZE];

bool probeTT (
    uint64_t key,
    int depth,
    int alpha, 
    int beta,
    int ply,
    TTEntry &TTOut,
    int &scoreOut,
    Move &moveOut
) {
    TTEntry &tt = TT[TTIndex(key)];

    // 如果 collide 直接跳掉
    if (tt.key != key) return false;
    // 如果是更淺就跳掉
    if (tt.depth < depth) return false;

    int score = tt.score;
    TTOut = tt;
    moveOut = tt.bestMove;

    if (score > MATE_SCORE - 1000) {
        score -= ply;
    } else if (score < -MATE_SCORE + 1000) {
        score += ply;
    }

    if (tt.flag == EXACT) {
        scoreOut = score;
        return true;
    }
    if (tt.flag == LOWER && score >= beta) {
        scoreOut = score;
        return true;
    }
    if (tt.flag == UPPER && score <= alpha) {
        scoreOut = score;
        return true;
    }

    return false;
}

void storeTT(
    uint64_t key,
    int depth,
    int ply,
    int score,
    TTFlag flag,
    Move bestMove
) {
    TTEntry &tt = TT[TTIndex(key)];
    // depth check，保留較深的 entry
    if (tt.key == key && depth < tt.depth) return;

    if (score >= MATE_SCORE - 1000) {
        score += ply;
    } else if (score <= -MATE_SCORE + 1000) {
        score -= ply;
    }

    tt.key = key;
    tt.depth = depth;
    tt.score = score;
    tt.flag = flag;
    tt.bestMove = bestMove;
}