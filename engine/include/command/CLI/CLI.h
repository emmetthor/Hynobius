#pragma once

#include "Engine.h"
#include <string>

void cliLoop(Engine &engine);

enum class RunMode
{
    NONE,
    PERFT,
    PERFT_DIVIDE,
    PERFT_STATS
};

struct Config
{
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int depth = 0;
    RunMode mode = RunMode::NONE;
};

Config parseArgs(int argc, char* argv[]);

int runCommand(const Config& cfg);