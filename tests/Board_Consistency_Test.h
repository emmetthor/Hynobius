#pragma once
#include "../engine/include/search/Board_Consistency.h"

#include <vector>
#include <string>
#include <iostream>

void testBoardConsistency(int depth, int testCount) {
    testCount = std::min(testCount, 2);
    std::vector<std::string> fenList = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
    };

    for (int i = 0; i < testCount; i++) {
        Board board = cinFenToBoard(fenList[i]);
        std::cout << "test " << i + 1 << " success with " << boardConsistency(board, depth) << " nodes.\n";
    }
}