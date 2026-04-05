
#include "../../engine/include/Engine.h"
#include "../../engine/include/UCI/UCI.h"
#include "Progress_Bar.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using TestData = std::pair<std::string, std::string>;
std::vector<TestData> readMateInOneFile(const std::string& filename)
{
    std::vector<TestData> res;

    std::ifstream infile(filename);
    if (infile.fail())
    {
        std::cout << "failed to open file: " << filename << '\n';
    }

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty())
            continue;

        std::istringstream ss(line);
        std::string fen, bestMove;

        std::getline(ss, fen, ',');
        std::getline(ss, bestMove, ',');

        res.push_back({fen, bestMove});
    }
    return res;
}

int main()
{
    auto testData = readMateInOneFile("../tests/bench/Mate_In_One.txt");

    int testCnt = 100;

    int failCnt = 0;
    int expectedFail = 0;
    progressBar bar(testCnt, 5);
    std::vector<std::array<std::string, 3>> failed;
    for (int i = 0; i < testCnt; i++)
    {
        auto [fen, bestMove] = testData[i];

        Engine engine;
        engine.setPositionWithFen(fen);

        Move move = engine.goDepth(2, 0);
        std::string retMove = UCIMoveToString(move);
        // 引擎應在兩半步內確認一步將殺

        if (retMove != bestMove)
        {
            if (fen == "1n2r3/1bnp2p1/r1Q4p/p3b2k/PP3p1P/2NB1K2/R2N1PP1/7R w - - 1 40")
            {
                expectedFail++;
            }
            else
            {
                failCnt++;
                failed.push_back({fen, bestMove, retMove});
            }
        }

        // bar.update(i + 1);
    }

    bar.finish();

    std::cout << "success test cases: " << testCnt - failCnt - expectedFail << " / " << testCnt
              << " [expected: " << expectedFail << ']' << '\n';
    std::cout << "failed test cases:\n";
    for (auto [a, b, c] : failed)
    {
        std::cout << a << " ; expected: " << b << " ; result: " << c << '\n';
    }

    ENGINE_ASSERT(failed.size() == 0);
}