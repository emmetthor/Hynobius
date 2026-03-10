#include "Perft_Test.h"
#include "Board_Consistency_Test.h"
#include "Bit_Move_Correctness.h"
#include <iostream>

int main() {
    std::cout << "========== Perft ==========\n";
    testAllPerft(6, 3);
    std::cout << "========== Board Consistency ==========\n";
    testBoardConsistency(3, 2);
    std::cout << "========== BitMove ==========\n";
    if (bruteMoveTest()) {
        std::cout << "success\n";
    }
}