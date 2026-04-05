#include "evaluate/Material_Point.h"

#include "board/Board.h"
#include "debug.h"

int pieceValue(const Piece p)
{
    switch (p)
    {
        case Piece::WKING:
        case Piece::BKING:
            break;

        case Piece::WQUEEN:
        case Piece::BQUEEN:
            return 900;

        case Piece::WKNIGHT:
        case Piece::BKNIGHT:
            return 320;

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            return 330;

        case Piece::WROOK:
        case Piece::BROOK:
            return 500;

        case Piece::WPAWN:
        case Piece::BPAWN:
            return 100;

        case Piece::EMPTY:
            return 0;

        case Piece::PIECE_COUNT:
            ENGINE_FATAL(DebugCategory::EVAL, "piece_count doesn't have a value.");
    }

    return 0;
}

int computePieceValue(const Board& board)
{
    int res = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board.board[r][c];
            if (p == Piece::EMPTY)
                continue;

            res += pieceValue(p) * (isWhite(p) ? 1 : -1);
        }
    }

    return res;
}