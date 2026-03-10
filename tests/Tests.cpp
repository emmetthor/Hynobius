#include "Perft_Test.h"
#include "Board_Consistency_Test.h"
#include <iostream>

int main() {
    std::cout << "========== Perft ==========\n";
    testAllPerft(6, 3);
    std::cout << "========== Board Consistency ==========\n";
    testBoardConsistency(3, 2);
}