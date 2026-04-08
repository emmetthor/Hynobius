#pragma once

#include "Time_Management.h"
#include "board/Board.h"
#include "evaluate/Evaluate.h"
#include "move/Move.h"
#include "search/Search.h"

class Engine
{
public:
    Engine();

    Evaluate eval;

    void setStartPosition();
    void setPositionWithFen(const std::string& fen);
    void checkReady();

    Move goDepth(int depth, bool isPrintInfo = 1);
    Move goClock(const TimeManage& tm);
    SearchResult fullInfoSearch(int depth);
    Board getBoard();
    void move(Move move);
    void quit();
    void changePlayer();
    void setPlayer(Player player);

private:
    Board board;
};