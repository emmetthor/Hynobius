#include "../../engine/include/Engine.h"
#include "Progress_Bar.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> readMateInTwoFile(const std::string& filename)
{
    std::vector<std::string> res;

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
        res.push_back(line);
    }
    return res;
}

int main()
{
    auto testData = readMateInTwoFile("../tests/bench/Mate_In_Two.txt");

    int testCnt = 20;

    int failCnt = 0;
    progressBar bar(testCnt, 5);
    std::vector<std::string> failed;
    for (int i = 0; i < testCnt; i++)
    {
        std::string fen = testData[i];

        Engine engine;
        engine.setPositionWithFen(fen);

        Evaluate eval;
        int score = engine.fullInfoSearch(4).bestScore;

        if (score != MATE_SCORE - 3)
        {
            failCnt++;
            failed.push_back(fen);
        }

        // bar.update(i + 1);
    }

    bar.finish();

    std::cout << "success test cases: " << testCnt - failCnt << " / " << testCnt << '\n';
    std::cout << "failed test cases:\n";
    for (auto s : failed)
    {
        std::cout << s << '\n';
    }

    assert(failCnt == 0);
}