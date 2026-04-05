#pragma once

#include "Search_Variables.h"
#include "board/Board.h"
#include "evaluate/Evaluate.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"

constexpr int MATE_SCORE = 1e6;

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

class Search
{
public:
    Search(Evaluate& _eval);
    SearchResult findBestMove(const Board& board, int depth);

    UndoState movestk[SearchVarialble::MAX_SEARCH_DEPTH];

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
};

void printInfo(SearchInfo info);