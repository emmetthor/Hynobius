#include "debug.h"
#include "fen/FEN_Parser.h"

#include <iostream>
#include <string>
#include <vector>

#include "board/Board.h"
#include "move/Generate_Move.h"
#include "move/Make_BitMove.h"
#include "search/Zobrist.h"

int boardConsistency(Board& board, int depth)
{
    if (depth <= 0)
        return 1;

    for (int pIndex = 1; pIndex <= 12; pIndex++)
    {
        Piece p = static_cast<Piece>(pIndex);
        for (int i = 0; i < board.getPieceCount(p); i++)
        {
            if (board.at(board.getPiecePos(p)[i]) != p)
            {
                ENGINE_FATAL(DebugCategory::BOARD, "different piece position");
            }
        }
    }

    int node = 0;

    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);

        node += boardConsistency(board, depth - 1);
        
        undoBitMove(board, move, undo);
    }

    return node;
}

int main()
{
    std::vector<std::pair<std::string, int>> fenList = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 8902}};

    for (int i = 0; i < (int)fenList.size(); i++)
    {
        auto [str, node] = fenList[i];
        Board board = cinFenToBoard(str);

        int res = boardConsistency(board, 3);
        if (res != node)
        {
            ENGINE_FATAL(DebugCategory::BOARD, "invalid node counts: ", res, ' ', node);
        }
        std::cout << "test " << i + 1 << " completed with " << node << " nodes.\n";
    }
}