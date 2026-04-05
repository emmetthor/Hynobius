#include "search/Search.h"
#include "board/Piece.h"
#include "move/Make_BitMove.h"

#pragma GCC optimize("O3,unroll-loops")

#include "Structure_IO.h"
#include "board/Board.h"
#include "board/Check.h"
#include "evaluate/Evaluate.h"
#include "evaluate/Material_Point.h"
#include "move/Generate_Move.h"
#include "move/Move.h"
#include "search/TT.h"
#include <chrono>

void Search::printMoveStk()
{
    for (int i = 0; i < backIterator; i++)
    {
        std::cout << moveStk[i] << ' ';
    }
    std::cout << '\n';
}

void printInfo(SearchInfo info)
{
    std::cout << "info depth " << info.depth << " nodes " << info.nodes << " qsNodes "
              << info.qsnodes << " times " << info.timeMs << " nps ";
    if (info.timeMs == 0)
        std::cout << "infinity";
    else
        std::cout << info.nodes * 1000 / info.timeMs;

    std::cout << " score " << info.score << '\n';
}

Search::Search(Evaluate& _eval)
{
    eval = _eval;
}

// find the best move of the current board. (base search)
SearchResult Search::findBestMove(const Board& board, int depth)
{
    auto startTime = std::chrono::steady_clock::now();

    Board copyBoard = board;
    SearchResult finalRes = {inValidMove, -INF};
    int lastScore = 0;

    for (int d = 1; d <= depth; d++)
    {
        SearchResult currentRes = {inValidMove, -INF};
        SearchInfo info;

        // LOG_DEBUG(DebugCategory::SEARCH, "depth: ", d);

        if (d == 1)
        {
            currentRes = searchRootCore(copyBoard, d, -INF, INF, finalRes.bestMove, 0);
        }
        else
        {
            // aspiration window
            int window = 30;
            int alpha = lastScore - window;
            int beta = lastScore + window;

            // alpha = -INF;
            // beta = INF;

            while (true)
            {
                currentRes = searchRootCore(copyBoard, d, alpha, beta, finalRes.bestMove, 0);

                int score = currentRes.bestScore;

                if (score <= alpha)
                {
                    alpha -= window;
                }
                else if (score >= beta)
                {
                    beta += window;
                }
                else
                {
                    break; // success
                }

                window *= 2;

                if (window > 2000)
                {
                    // if it can't find, give it the widest window.
                    alpha = -INF;
                    beta = INF;
                }
            }
        }

        lastScore = currentRes.bestScore;
        finalRes = currentRes;

        auto endTime = std::chrono::steady_clock::now();

        finalRes.info.depth = d;
        finalRes.info.nodes = negamaxNodes + qsNodes;
        finalRes.info.qsnodes = qsNodes;
        finalRes.info.timeMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        finalRes.info.score = finalRes.bestScore;
    }

    return finalRes;
}

// search with different alpha and beta basis. (core search)
SearchResult
Search::searchRootCore(Board& board, int depth, int alpha, int beta, Move iterativeMove, int ply)
{
    SearchResult res;
    res.bestScore = -INF;

    // generate all moves
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    // WARN removed
    // sort moves
    // advanceMoves adv = {iterativeMove, killerMove[0][ply], killerMove[1][ply]};
    // sortMove(board, moves, nMoves, adv);

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        // recursive
        doBitMove(board, move, undo);

        int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);

        undoBitMove(board, move, undo);

        // LOG_DEBUG(DebugCategory::SEARCH, "move: ", move, " | move score: ", score);

        if (score > res.bestScore)
        {
            res.bestMove = bitMovetoOriMove(board, move);
            res.bestScore = score;
        }
    }

    return res;
}

int Search::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    negamaxNodes++;

    // tt table
    ttProbe++;
    int ttScore = 0;
    TTEntry tt;
    Move ttMove;
    if (probeTT(board.zobristKey, depth, alpha, beta, ply, tt, ttScore, ttMove))
    {
        ttHit++;
        return ttScore;
    }

    int oriAlpha = alpha;
    int bestScore = -INF;
    Move bestMove;
    bool hasMove = 0;

    // depth = 0 -> qs search
    if (depth == 0)
    {
        // return (player == Player::WHITE ? 1 : -1) * eval.evaluateBoard(board,
        // EVALUATE_MODE::FULL);
        return quietscence(board, alpha, beta, ply);
    }

    // generate all moves
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);
    totalMoves += nMoves;

    // check checkmate / stalemate
    if (nMoves == 0)
    {
        // LOG_DEBUG(DebugCategory::SEARCH, "no move!");
        // std::cout << "checkmate\n" << board << '\n';
        if (isInCheck(board, board.player))
            return -MATE_SCORE + ply;
        else
            return 0;
    }

    // WARN removed
    // advanceMoves adv = {ttMove, killerMove[0][ply], killerMove[1][ply]};
    // sortMove(board, moves, nMoves, adv);

    for (int i = 0; i < nMoves; i++)
    {
        int searchDepth = depth - 1;

        BitMove move = moves[i];

        UndoState undo;

        int score = 0;

        // recursive
        doBitMove(board, move, undo);

        if (i == 0)
        {
            // search fully in the first move (it may be the best move while setting a range of
            // alpha and beta for other moves.)
            score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        }
        else
        {
            score = -negamax(board, searchDepth, -alpha - 1, -alpha, ply + 1);
            if (score > alpha)
            {
                score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
            }
        }

        undoBitMove(board, move, undo);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = bitMovetoOriMove(board, move);
        }

        if (score > alpha)
        {
            alpha = score;
        }

        if (alpha >= beta)
        {
            // LOG_DEBUG(DebugCategory::SEARCH, "negamax cut | score=", score, " | alpha=", alpha, "
            // | beta=", beta);
            break;
        }
    }

    TTFlag flag;
    if (alpha <= oriAlpha)
        flag = UPPER;
    else if (alpha >= beta)
        flag = LOWER;
    else
        flag = EXACT;

    storeTT(board.zobristKey, depth, ply, alpha, flag, (hasMove ? bestMove : inValidMove));

    return bestScore;
}

// quietscence search (determine time complexity)
int Search::quietscence(Board& board, int alpha, int beta, int ply)
{
    qsNodes++;
    int standerdPoint =
        (board.player == Player::WHITE ? 1 : -1) * eval.evaluateBoard(board, EVALUATE_MODE::FULL);
    if (standerdPoint >= beta)
        return standerdPoint;
    if (standerdPoint > alpha)
        alpha = standerdPoint;

    BitMove captureMoves[256];
    int nCaptureMoves = generateLegalCaptureMoves(board, captureMoves);

    // WARN removed
    // advanceMoves adv = {inValidMove, killerMove[0][ply], killerMove[1][ply]};
    // sortMove(board, captureMoves, nCaptureMoves, adv);

    if (nCaptureMoves == 0)
    {
        return alpha;
    }

    for (int i = 0; i < nCaptureMoves; i++)
    {
        BitMove move = captureMoves[i];

        MoveState state(board, move);

        // delta pruning
        Piece captured = state.capturedPiece;
        ENGINE_ASSERT(!(!state.isPromotion && captured == Piece::EMPTY));
        if (!state.isPromotion && pieceValue(state.movePiece) >= pieceValue(captured) + 200)
            continue;

        UndoState undo;

        // recursive
        doBitMove(board, move, undo);

        int score = 0;

        score = -quietscence(board, -beta, -alpha, ply + 1);

        undoBitMove(board, move, undo);

        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}