#include "Time_Management.h"

int64_t timeManageCore(int64_t timeLeft, int64_t increment)
{
    return timeLeft / 30 + increment;
}

SearchLimits timeManager(const TimeManage& tm, Player player)
{
    if (tm.depth != -1)
        return {tm.depth, MAX_THINK_TIME};

    int64_t thinkTime = (player == Player::WHITE ? timeManageCore(tm.wtime, tm.winc)
                                                 : timeManageCore(tm.btime, tm.binc));

    return {-1, thinkTime};
}