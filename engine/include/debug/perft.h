#pragma once

#include <cstdint>
#include "board/Board.h"

struct PerftStats
{
    uint64_t nodes = 0;
    uint64_t captures = 0;
    uint64_t enPassants = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
};

inline PerftStats& operator+=(PerftStats& a, const PerftStats& b)
{
    a.nodes += b.nodes;
    a.captures += b.captures;
    a.enPassants += b.enPassants;
    a.castles += b.castles;
    a.promotions += b.promotions;
    a.checks += b.checks;
    return a;
}

inline PerftStats operator+(PerftStats a, const PerftStats& b)
{
    a += b;
    return a;
}

uint64_t perft(Board& board, int depth);

PerftStats perftWithStat(Board& board, int depth);