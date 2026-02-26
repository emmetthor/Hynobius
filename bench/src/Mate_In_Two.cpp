#include "Mate_In_Two.h"
#include "Progress_Bar.h"
#include "Bench.h"
#include "../../engine/include/Engine.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <iostream>

std::vector<std::string> readMateInTwoFile(const std::string &filename) {
    std::vector<std::string> res;

    std::ifstream infile(filename);
    if (infile.fail()) {
        std::cout << "failed to open file: " << filename << '\n';
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        res.push_back(line);
    }
    return res;
}

testResult testMateInTwo(int testCnt) {
    auto testData = readMateInTwoFile("../bench/Mate_In_Two.txt");

    testCnt = std::min(testCnt, (int)testData.size());

    int failCnt = 0;
    progressBar bar(testCnt, 5);
    std::vector<std::string> failed;
    for (int i = 0; i < testCnt; i++) {
        std::string fen = testData[i];

        Engine engine;
        engine.setPositionWithFen(fen);
        
        Evaluate eval;
        int score = engine.fullInfoSearch(4).bestScore;

        if (score != MATE_SCORE - 3) {
            failCnt++;
            failed.push_back(fen);
        }

        //bar.update(i + 1);
    }

    bar.finish();

    std::cout << "success test cases: " << testCnt - failCnt << " / " << testCnt << '\n';
    std::cout << "failed test cases:\n";
    for (auto s : failed) {
        std::cout << s << '\n';
    }

    return {testCnt, failCnt, 0};
}