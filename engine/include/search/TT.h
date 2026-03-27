#pragma once

#include "move/Move.h"

#include <cstdint>

constexpr int TT_SIZE = 1 << 23;
constexpr uint64_t TT_MASK = TT_SIZE - 1;

enum TTFlag
{
    EXACT,
    LOWER,
    UPPER
};

struct TTEntry
{
    uint64_t key;
    int depth;
    int score;
    TTFlag flag;
    Move bestMove;
};

extern TTEntry TT[TT_SIZE];

inline int TTIndex(uint64_t key)
{
    return key & TT_MASK;
}

bool probeTT(uint64_t key,
             int depth,
             int alpha,
             int beta,
             int ply,
             TTEntry& TTOut,
             int& scoreOut,
             Move& moveOut);

void storeTT(uint64_t key, int depth, int ply, int score, TTFlag flag, Move bestMove);