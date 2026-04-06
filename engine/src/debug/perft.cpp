#include "debug/perft.h"
#include "move/Generate_Move.h"
#include "board/Check.h"
#include "move/Make_BitMove.h"
#include "debug/validation.h"

PerftStats perftWithStat(Board& board, int depth)
{
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    PerftStats stat;

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);

        if (depth == 1)
        {
            stat.nodes++;
            stat.captures += (getCapture(move) == true ? 1 : 0);
            stat.enPassants = 0; // WARN en passants are not implemented yet.
            stat.castles += (getCastle(move) == true ? 1 : 0);
            stat.promotions += (getPromotion(move) == true ? 1 : 0);
            // player now is the enemy because we've functioned doBitMove
            stat.checks += (isInCheck(board, board.player) ? 1 : 0);
        }
        else
        {
            stat += perftWithStat(board, depth - 1);
        }

        undoBitMove(board, move, undo);

        //checkBoardState(board);
    }

    return stat;
}

uint64_t perft(Board& board, int depth)
{
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    int node = 0;

    if (depth == 0)
    {
        return 1;
    }
    if (depth == 1)
    {
        return nMoves;
    }

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);

        node += perft(board, depth - 1);

        undoBitMove(board, move, undo);

        checkBoardState(board);
    }

    return node;
}