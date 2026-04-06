#include "debug/validation.h"
#include "fen/FEN_Parser.h"

#include <fstream>
#include <iostream>
#include <string>

#include "board/Board.h"
#include "move/Generate_Move.h"
#include "move/Make_BitMove.h"

int boardConsistency(Board& board, int depth)
{

    int node = 0;

    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    if (depth == 1) {
        return nMoves;
    }

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);
        checkBoardState(board);

        node += boardConsistency(board, depth - 1);

        undoBitMove(board, move, undo);
        checkBoardState(board);
    }

    return node;
}

int main()
{
    std::ifstream fin("../tests/board_consistency/Board_Consistency.txt");
    if (!fin.is_open())
    {
        ENGINE_FATAL("board consistency", "failed to open FEN file");
    }

    std::string fen;
    int caseId = 0;
    int depth = 3;

    while (std::getline(fin, fen))
    {
        if (fen.empty())
            continue;

        caseId++;

        Board board = cinFenToBoard(fen);
        int nodes = boardConsistency(board, depth);

        std::cout << "case " << caseId << " passed, nodes = " << nodes << '\n';
    }

    std::cout << "all FEN cases passed.\n";
}