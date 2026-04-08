#pragma once

#include "search/Search.h"
#include <cstdint>

constexpr int64_t MAX_THINK_TIME = INT64_MAX;

struct TimeManage
{
    int64_t wtime = -1, btime = -1, winc = 0, binc = 0;
    int depth = -1;
};

SearchLimits timeManager(const TimeManage& tm, Player player);