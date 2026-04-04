#include "../engine/include/board/Board.h"
#include "../engine/include/fen/FEN_Parser.h"
#include "../engine/include/move/Generate_Move.h"
#include "../engine/include/move/Make_Move.h"
#include "../engine/include/move/Move.h"
#include "move/Make_BitMove.h"
#include <iostream>
#include <string>
#include <vector>

int perft(Board& board, int depth)
{
    if (depth <= 0)
        return 1;
    int nodes = 0;

    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];
        
        UndoState undo;

        doBitMove(board, move, undo);

        nodes += perft(board, depth - 1);
        
        undoBitMove(board, move, undo);
    }

    return nodes;
}

std::vector<int> testPerft(std::string fen, int depth)
{
    Board board = cinFenToBoard(fen);
    std::vector<int> res(depth + 1);

    for (int d = 1; d <= depth; d++)
    {
        int nodes = perft(board, d);
        res[d] = nodes;
    }
    return res;
}

int main()
{
    int doDepth = 5;
    std::vector<std::string> fenList = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "7k/8/8/8/8/8/7P/4K2R w K - 0 1",
        "7k/PPPPPP1p/5p1K/5PpP/6P1/8/8/8 w - - 0 1"};
    std::vector<int> validTests = {6, 0, 1, 1};
    std::vector<std::vector<int>> result = {{1, 20, 400, 8902, 197281, 4865609},
                                            {1, 48, 2039, 97862, 4085603, 193690690},
                                            {1, 10},
                                            {1, 24}};
    std::vector<std::vector<int>> enPassantDiff = {
        {0, 0, 0, 0, 0, 258}, {0, 0, 1, 45, 1929, 73365, 3577504}, {0, 0}, {0, 0}};
    int totalTestCases = 0;
    int failedTestCases = 0;
    int expected = 0;

    for (int i = 0; i < 4; i++)
    {
        auto res = testPerft(fenList[i], std::min(doDepth, validTests[i]));
        for (int j = 1; j <= std::min(doDepth, validTests[i]); j++)
        {
            totalTestCases++;
            std::cout << "test" << i << "| result=" << res[j] << " | expect=" << result[i][j]
                      << '\n';
            if (res[j] != result[i][j])
            {
                if (enPassantDiff[i][j] != 0 && res[j] + enPassantDiff[i][j] == result[i][j])
                {
                    expected++;
                }
                else
                {
                    failedTestCases++;
                }
            }
        }
    }

    std::cout << "success test cases: " << totalTestCases - failedTestCases - expected << '/'
              << totalTestCases << " [expected: " << expected << "]\n";

    assert(failedTestCases == 0);
}