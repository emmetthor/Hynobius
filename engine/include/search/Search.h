#pragma once

#include "Search_Variables.h"
#include "board/Board.h"
#include "evaluate/Evaluate.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include <chrono>

constexpr int MATE_SCORE = 1e6;
constexpr int TIMEOUT_SCORE = 12345678;

struct SearchInfo
{
    int depth;
    int score;
    int nodes;
    int qsnodes;
    int timeMs;
    int nps;
};

struct SearchResult
{
    Move bestMove;
    int bestScore;
    SearchInfo info;
};

struct SearchLimits
{
    int maxDepth = -1;
    int64_t maxTimeMs = -1;
};

class Search
{
public:
    Search(Evaluate& _eval);
    SearchResult findBestMove(const Board& board);

    UndoState movestk[SearchVarialble::MAX_SEARCH_DEPTH];

    inline void setSearchLimits(SearchLimits _limits)
    {
        limits = _limits;
    }

private:
    Evaluate eval;

    int quietscence(Board& board, int alpha, int beta, int ply);

    int negamax(Board& board, int depth, int alpha, int beta, int ply);

    SearchResult
    searchRootCore(Board& board, int depth, int alpha, int beta, Move iterativeMove, int ply);

    void printMoveStk();

    int negamaxNodes = 0;
    int qsNodes = 0;
    int ttProbe = 0;
    int ttHit = 0;
    int ttCut = 0;
    int totalMoves = 0;
    int betaCutAtMove[5] = {};
    const int INF = 1e9;
    Move moveStk[10];
    int backIterator = 0;

    SearchLimits limits;

    struct SearchState
    {
        bool stopped = false;
        bool timeout = false;

        uint64_t negamaxNodes = 0;
        uint64_t qsNodes = 0;

        std::chrono::steady_clock::time_point startTime;
    } state;

    bool shouldStop();
};

void printInfo(SearchInfo info);