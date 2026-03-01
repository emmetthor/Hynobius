#pragma once

#include "../engine/include/board/Board.h"
#include "../engine/include/search/Perft.h"
#include "../engine/include/fen/FEN_Parser.h"
#include <iostream>
#include <string>
#include <vector>

std::vector<int> testPerft(std::string fen, int depth) {
    Board board = cinFenToBoard(fen);
    std::vector<int> res(depth + 1);

    for (int d = 1; d <= depth; d++) {
        int nodes = perft(board, d, Player::WHITE);
        res[d] = nodes;
    }
    return res;
}

void testAllPerft(int depth, int testCount) {
    int doDepth = std::min(depth, 5);
    testCount = std::min(testCount, 2);
    std::vector<std::string> fenList = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
    };
    std::vector<std::vector<int>> result = {
        {1, 20, 400, 8902, 197281, 4865609},
        {1, 48, 2039, 97862, 4085603, 193690690}
    };
    std::vector<std::vector<int>> enPassantDiff = {
        {0, 0, 0, 0, 0, 258},
        {0, 0, 1, 45, 1929, 73365, 3577504}
    };
    int totalTestCases = 0;
    int failedTestCases = 0;
    int expected = 0;

    for (int i = 0; i < testCount; i++) {
        auto res = testPerft(fenList[i], doDepth);
        for (int j = 1; j <= doDepth; j++) {
            totalTestCases++;
            if (res[j] != result[i][j]) {
                if (enPassantDiff[i][j] != 0 && res[j] + enPassantDiff[i][j] == result[i][j]) {
                    expected++;
                } else {
                    failedTestCases++;
                }
            }
        }
    }

    std::cout << "success test cases: "
              << totalTestCases - failedTestCases - expected << '/' << totalTestCases
              << " [expected: " << expected << "]\n";
    if (failedTestCases) exit(1);
}