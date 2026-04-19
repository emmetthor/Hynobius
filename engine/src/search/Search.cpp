#include "search/Search.h"
#include "Structure_IO.h"
#include "board/Board.h"
#include "board/Check.h"
#include "evaluate/Evaluate.h"
#include "move/Generate_Move.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "move/Move_Order.h"
#include "search/Search_Variables.h"
#include "search/TT.h"
#include <chrono>

void printInfo(const SearchInfo& info)
{
    std::cout << "info depth " << info.depth;

    if (info.score >= MATE_SCORE - SearchVarialble::MAX_SEARCH_DEPTH)
    {
        int mate = (MATE_SCORE - info.score + 1) / 2;
        std::cout << " score mate " << mate;
    }
    else if (info.score <= -MATE_SCORE + SearchVarialble::MAX_SEARCH_DEPTH)
    {
        int mate = (MATE_SCORE + info.score + 1) / 2;
        std::cout << " score mate -" << mate;
    }
    else
    {
        std::cout << " score cp " << info.score;
    }

    std::cout << " nodes " << info.nodes << " nps " << info.nps << " time " << info.timeMs;

    std::cout << " pv ";
    int pvLength = info.pv.length[0];
    for (int i = 0; i < pvLength; i++)
    {
        std::cout << bitMoveToUCIMove(info.pv.table[0][i]) << ' ';
    }

    std::cout << '\n';
}

bool Search::shouldStop()
{
    if (state.stopped)
        return true;

    if (limits.maxTimeMs != -1)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - state.startTime).count();

        if (elapsed >= limits.maxTimeMs)
        {
            state.stopped = true;
            state.timeout = true;
            return true;
        }
    }

    return false;
}

// init function
Search::Search(const Evaluate& _eval, const SearchLimits _limits)
{
    eval = _eval;
    limits = _limits;
}

// public function to search for moves.
SearchResult Search::findBestMove(const Board& board)
{
    // init state.
    state = {false, false, 0, 0, std::chrono::steady_clock::now()};

    // make copyBoard non-const.
    Board copyBoard = board;

    // Init repetition history
    copyBoard.pushRepetitionKey();

    // current result is invalid
    SearchResult result = {false, inValidMove, -MAX_SCORE, INVALID_BITMOVE};

    // At least output a valid move
    BitMove rootMoves[256];
    int nRootMoves = generateAllLegalMoves(copyBoard, rootMoves);
    if (nRootMoves > 0)
    {
        result.isValid = true;
        result.bestMove = bitMovetoOriMove(copyBoard, rootMoves[0]);
        result.bestScore = -MAX_SCORE;
    }

    // set max depth.
    int maxDepth = limits.maxDepth == -1 ? SearchVarialble::MAX_SEARCH_DEPTH : limits.maxDepth;

    BitMove lastBestMove = INVALID_BITMOVE;
    // iterative deepening
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        // check time.
        if (shouldStop())
            break;

        SearchResult currentResult;
        if (depth == 1)
        {
            currentResult = chooseMove(copyBoard, depth, -MAX_SCORE, MAX_SCORE, 0, lastBestMove);
        }
        else
        {
            int window = 16;

            while (true)
            {
                int alpha = state.prevScore - window;
                int beta = state.prevScore + window;

                currentResult = chooseMove(copyBoard, depth, alpha, beta, 0, lastBestMove);

                if (!currentResult.isValid)
                    return result;

                if (currentResult.bestScore <= alpha)
                {
                    window *= 2;
                    continue;
                }
                if (currentResult.bestScore >= beta)
                {
                    window *= 2;
                    continue;
                }

                break;
            }
        }

        if (currentResult.isValid)
        {
            result = currentResult;
            lastBestMove = result.bestBitMove;
            state.prevPv = state.pv;
            state.prevScore = result.bestScore;
        }

        // print info
        SearchInfo info;
        info.depth = depth;
        info.score = result.bestScore;
        info.nodes = state.negamaxNodes + state.qsNodes;
        info.qsnodes = state.qsNodes;

        auto now = std::chrono::steady_clock::now();
        info.timeMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - state.startTime).count();
        info.nps = (info.timeMs > 0 ? info.nodes * 1000 / info.timeMs : 0);
        info.pv = state.pv;

        printInfo(info);
    }

    copyBoard.popRepetitionKey();

    return result;
}

SearchResult
Search::chooseMove(Board& board, int depth, int alpha, int beta, int ply, const BitMove PVMove)
{
    SearchResult result = {false, inValidMove, -MAX_SCORE};

    // Clear currecnt PV line
    state.pv.clearLine(ply);

    // generate all moves
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    // Get last PV move
    BitMove pvMove = INVALID_BITMOVE;
    if (state.prevPv.length[ply] > 0)
        pvMove = state.prevPv.table[ply][0];

    // sort moves
    advanceMoves adv = {
        pvMove, INVALID_BITMOVE, state.kill.table[0][ply], state.kill.table[1][ply]};
    sortMove(board, moves, nMoves, adv);

    for (int i = 0; i < nMoves; i++)
    {
        // time check.
        if (shouldStop())
        {
            board.popRepetitionKey();
            return {false, inValidMove, -MAX_SCORE, INVALID_BITMOVE};
        }

        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);
        board.pushRepetitionKey();

        int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);

        board.popRepetitionKey();
        undoBitMove(board, move, undo);

        Move oriMove = bitMovetoOriMove(board, move);
        // std::cout << oriMove << " | " << score << '\n';

        if (score == -TIMEOUT_SCORE)
        {
            board.popRepetitionKey();
            return {false, inValidMove, -MAX_SCORE, INVALID_BITMOVE};
        }
        if (score > result.bestScore)
        {
            result.isValid = true;
            result.bestMove = bitMovetoOriMove(board, move);
            result.bestScore = score;
            result.bestBitMove = move;
        }
        if (score > alpha)
        {
            alpha = score;
            state.pv.update(ply, move);
        }
    }

    return result;
}

int Search::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    state.negamaxNodes++;

    // Clear current PV line
    state.pv.clearLine(ply);

    // Check repetition
    if (isRepetition(board))
        return 0;

    // Probe TT table.
    TTEntry ttOut;
    int ttScore = -MAX_SCORE;
    BitMove ttMove = INVALID_BITMOVE;
    if (probeTT(board.zobristKey, depth, alpha, beta, ply, ttOut, ttScore, ttMove) == true)
    {
        return ttScore;
    }

    // Record original alpha for TT store.
    int oriAlpha = alpha;

    int bestScore = -MAX_SCORE;
    BitMove bestMove = INVALID_BITMOVE;
    bool hasMove = false;

    // time check.
    if ((state.negamaxNodes + state.qsNodes) & 2047)
    {
        // continue searching
    }
    else
    {
        // check time
        if (shouldStop())
            return TIMEOUT_SCORE;
    }

    // depth = 0 -> qs search
    if (depth == 0)
    {
        return quietscence(board, alpha, beta, ply);
    }

    // generate all moves
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    // Get last PV move.
    BitMove pvMove = INVALID_BITMOVE;
    if (state.prevPv.length[ply] > 0)
        pvMove = state.prevPv.table[ply][0];

    // Sort moves.
    advanceMoves adv = {pvMove, ttMove, state.kill.table[0][ply], state.kill.table[1][ply]};
    sortMove(board, moves, nMoves, adv);

    // check checkmate / stalemate
    if (nMoves == 0)
    {
        if (isInCheck(board, board.player))
            return -MATE_SCORE + ply;
        else
            return 0;
    }

    for (int i = 0; i < nMoves; i++)
    {
        // time check.
        if (shouldStop())
            return TIMEOUT_SCORE;

        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);
        board.pushRepetitionKey();

        bool doLMR = false;

        if (i >= 4 &&          // after the fifth move -> reduce search depth.
            depth >= 3 &&      // LMR is only for deep nodes.
            !undo.isCapture && // don't reduce capture moves.
            !undo.isPromotion  // don't reduce promotions.
        )
        {
            // after doBitMove, the player stored in board is already the enemy.
            if (!isInCheck(board, board.player))
            {
                doLMR = true;
            }
        }

        int score = -MAX_SCORE;

        if (doLMR)
        {
            int searchDepth = depth - 2;

            // Using null-window to limit score window -> faster.
            score = -negamax(board, searchDepth, -alpha - 1, -alpha, ply + 1);
            if (score > alpha)
            {
                // fail high -> research with full depth.
                score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
            }
        }
        else
        {
            score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        }

        board.popRepetitionKey();
        undoBitMove(board, move, undo);

        // time check
        if (score == -TIMEOUT_SCORE)
            return TIMEOUT_SCORE;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
            hasMove = true;
        }
        if (score > alpha)
        {
            alpha = score;

            // Update PV line
            state.pv.update(ply, move);
        }

        if (alpha >= beta)
        {
            if (!undo.isCapture && !undo.isPromotion)
            {
                state.kill.addKillerMove(move, ply);
            }
            break;
        }
    }

    // define TT flag to store.
    TTFlag flag;
    if (bestScore <= oriAlpha)
        flag = UPPER;
    else if (bestScore >= beta)
        flag = LOWER;
    else
        flag = EXACT;

    storeTT(board.zobristKey, depth, ply, bestScore, flag, bestMove);
    // store to TT table.

    return bestScore;
}

// quietscence search (determine time complexity)
int Search::quietscence(Board& board, int alpha, int beta, int ply)
{
    state.qsNodes++;

    // time check.
    if ((state.negamaxNodes + state.qsNodes) & 2047)
    {
        // continue searching
    }
    else
    {
        // check
        if (shouldStop())
            return TIMEOUT_SCORE;
    }

    int standerdPoint =
        (board.player == Player::WHITE ? 1 : -1) * eval.evaluateBoard(board, EVALUATE_MODE::FULL);
    if (standerdPoint >= beta)
        return standerdPoint;
    if (standerdPoint > alpha)
        alpha = standerdPoint;

    BitMove captureMoves[256];
    int nCaptureMoves = generateLegalCaptureMoves(board, captureMoves);

    // Sort moves.
    advanceMoves adv = {
        INVALID_BITMOVE,
        INVALID_BITMOVE,
        INVALID_BITMOVE,
        INVALID_BITMOVE,
    };
    sortMove(board, captureMoves, nCaptureMoves, adv);

    if (nCaptureMoves == 0)
    {
        return alpha;
    }

    for (int i = 0; i < nCaptureMoves; i++)
    {
        BitMove move = captureMoves[i];

        UndoState undo;

        // recursive
        doBitMove(board, move, undo);

        int score = -quietscence(board, -beta, -alpha, ply + 1);

        undoBitMove(board, move, undo);

        // time check.
        if (score == -TIMEOUT_SCORE)
            return TIMEOUT_SCORE;

        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}