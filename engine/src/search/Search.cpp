#include "search/Search.h"

#pragma GCC optimize("O3,unroll-loops")

#include "Structure_IO.h"
#include "board/Board.h"
#include "board/Check.h"
#include "debug.h"
#include "evaluate/Evaluate.h"
#include "evaluate/Material_Point.h"
#include "move/Generate_Move.h"
#include "move/Make_Move.h"
#include "move/Move.h"
#include "move/Move_Order.h"
#include "search/Killer_Move.h"
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

                window *= 2; // 指數成長

                if (window > 2000)
                {
                    alpha = -INF;
                    beta = INF;
                    // 太多次還沒找到，就給他最大窗口
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

SearchResult
Search::searchRootCore(Board& board, int depth, int alpha, int beta, Move iterativeMove, int ply)
{
    SearchResult res;
    res.bestScore = -INF;

    // 生成所有走法
    Move moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    // 排序
    advanceMoves adv = {iterativeMove, killerMove[0][ply], killerMove[1][ply]};
    sortMove(board, moves, nMoves, adv);

    for (int i = 0; i < nMoves; i++)
    {
        Move move = moves[i];

        moveStk[backIterator++] = move;

        // 遞迴下一層
        makeMove(board, move);

        int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);

        undoMove(board, move);

        backIterator--;

        // LOG_DEBUG(DebugCategory::SEARCH, "move: ", move, " | move score: ", score);

        if (score > res.bestScore)
        {
            res.bestMove = move;
            res.bestScore = score;
        }
    }

    return res;
}

int Search::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    negamaxNodes++;

    // TT表 記憶化搜索
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

    // depth = 0 進入QS
    if (depth == 0)
    {
        // return (player == Player::WHITE ? 1 : -1) * eval.evaluateBoard(board,
        // EVALUATE_MODE::FULL);
        return quietscence(board, alpha, beta, ply);
    }

    // 生成所有走法
    Move moves[256];
    int nMoves = generateAllLegalMoves(board, moves);
    totalMoves += nMoves;

    // 檢查 checkmate / stalemate
    if (nMoves == 0)
    {
        // LOG_DEBUG(DebugCategory::SEARCH, "no move!");
        // std::cout << "checkmate\n" << board << '\n';
        if (isInCheck(board, board.player))
            return -MATE_SCORE + ply;
        else
            return 0;
    }

    advanceMoves adv = {ttMove, killerMove[0][ply], killerMove[1][ply]};
    sortMove(board, moves, nMoves, adv);

    for (int i = 0; i < nMoves; i++)
    {
        int searchDepth = depth - 1;
        Move move = moves[i];

        moveStk[backIterator++] = move;

        // if (!move.isPromotion && move.capturePiece == Piece::EMPTY && depth >= 3 &&
        // !isInCheck(board, player)) {
        //     if (i >= 4) {
        //         searchDepth--;
        //     }

        //     if (i >= 7) {
        //         searchDepth--;
        //     }
        // }

        int score = 0;

        makeMove(board, move);

        // score = -negamax(
        //     board,
        //     depth - 1,
        //     -beta,
        //     -alpha,
        //     opponent(player),
        //     ply + 1
        // );

        if (i == 0)
        {
            // 第一步全搜
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

        undoMove(board, move);

        backIterator--;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
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

int Search::quietscence(Board& board, int alpha, int beta, int ply)
{
    qsNodes++;
    int standerdPoint =
        (board.player == Player::WHITE ? 1 : -1) * eval.evaluateBoard(board, EVALUATE_MODE::FULL);
    if (standerdPoint >= beta)
        return standerdPoint;
    if (standerdPoint > alpha)
        alpha = standerdPoint;

    Move captureMoves[256];
    int nCaptureMoves = generateLegalCaptureMoves(board, captureMoves);

    advanceMoves adv = {inValidMove, killerMove[0][ply], killerMove[1][ply]};

    sortMove(board, captureMoves, nCaptureMoves, adv);

    if (nCaptureMoves == 0)
    {
        return alpha;
    }

    for (int i = 0; i < nCaptureMoves; i++)
    {
        Move move = captureMoves[i];

        // delta pruning 如果吃子太糟就直接跳過
        Piece captured = move.capturePiece;
        ENGINE_ASSERT(!(!move.isPromotion && captured == Piece::EMPTY));
        if (!move.isPromotion && pieceValue(move.movePiece) >= pieceValue(captured) + 200)
            continue;

        makeMove(board, move);

        int score = 0;
        ENGINE_ASSERT(!isInCheck(board, player));
        score = -quietscence(board, -beta, -alpha, ply + 1);

        undoMove(board, move);

        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}