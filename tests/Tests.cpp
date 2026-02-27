#include "Bit_Move_Correctness.h"

#include <iostream>

int main() {
    std::cout << "========== BitMove ==========\n";
    if (bruteMoveTest()) {
        std::cout << "success\n";
    }
}