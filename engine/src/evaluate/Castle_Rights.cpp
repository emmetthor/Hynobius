#include "evaluate/Castle_Rights.h"
#include "board/Board.h"

int evaluateCastleRights(const Board& board, int castleWeight)
{
    int res = 0;
    res +=
        castleWeight *
        (((board.castleRights & 0b1000) != 0 ? 1 : 0) + ((board.castleRights & 0b0100) != 0 ? 1 : 0) +
         ((board.castleRights & 0b0010) != 0 ? -1 : 0) + ((board.castleRights & 0b0001) != 0 ? -1 : 0));
    return res;
}