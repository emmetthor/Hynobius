#include "evaluate/Pawn_Structure.h"
#include "board/Board.h"

int fileCount[2][8] = {};

int doublePawn(const Board& board, Player player, const int doublePawnWeight)
{
    int index = playerToIndex(player);

    int cnt = 0;
    for (int c = 0; c < 8; c++)
    {
        if (fileCount[index][c] > 1)
        {
            cnt += fileCount[index][c] - 1;
        }
    }

    return -cnt * doublePawnWeight;
}

int isolatePawn(const Board& board, Player player, const int isolatedPawnWeight)
{
    int index = playerToIndex(player);

    int cnt = 0;
    for (int c = 0; c < 8; c++)
    {
        if (fileCount[index][c] == 0)
            continue;

        bool isIsolated = 1;
        if (c != 0)
            if (fileCount[index][c - 1] != 0)
                isIsolated = 0;
        if (c != 7)
            if (fileCount[index][c + 1] != 0)
                isIsolated = 0;

        if (isIsolated)
            cnt += fileCount[index][c];
    }

    return -cnt * isolatedPawnWeight;
}

int passedPawn(const Board& board,
               Player player,
               const int passPawnWeight,
               const int* passPawnRankWeight)
{
    int index = playerToIndex(player);
    int oppoIndex = playerToIndex(opponent(player));
    Piece myPawn = makePiece(player, 'P');
    Piece oppoPawn = makePiece(opponent(player), 'P');
    int myPawnCnt = board.getPieceCount(myPawn);
    int oppoPawnCnt = board.getPieceCount(oppoPawn);
    const auto* myPawnArray = board.getPiecePos(myPawn);
    const auto* oppoPawnArray = board.getPiecePos(oppoPawn);
    int dir = (player == Player::WHITE ? -1 : 1);
    int targetRank = (player == Player::WHITE ? 0 : 7);

    int res = 0;
    for (int i = 0; i < myPawnCnt; i++)
    {
        auto [r, c] = myPawnArray[i];
        bool isPassedPawn = 1;
        for (int j = 0; j < oppoPawnCnt; j++)
        {
            auto [oppoR, oppoC] = oppoPawnArray[j];
            if ((oppoC == c - 1 || oppoC == c + 1 || oppoC == c) && oppoR * dir > r * dir)
            {
                isPassedPawn = 0;
                break;
            }
        }

        if (!isPassedPawn)
            continue;
        res += passPawnWeight + passPawnRankWeight[abs(targetRank - r)];
    }

    return res;
}

int evaluatePawnStructure(const Board& board,
                          const int doublePawnWeight,
                          const int isolatedPawnWeight,
                          const int passedPawnWeight,
                          const int* passedPawnRankWeight)
{
    Piece wp = makePiece(Player::WHITE, 'P');
    Piece bp = makePiece(Player::BLACK, 'P');

    int wpCnt = board.getPieceCount(wp);
    int bpCnt = board.getPieceCount(bp);

    const auto* wpArray = board.getPiecePos(wp);
    const auto* bpArray = board.getPiecePos(bp);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            fileCount[i][j] = 0;
        }
    }

    for (int i = 0; i < wpCnt; i++)
    {
        fileCount[0][wpArray[i].col]++;
    }

    for (int i = 0; i < bpCnt; i++)
    {
        fileCount[1][bpArray[i].col]++;
    }

    int res = 0;
    res += doublePawn(board, Player::WHITE, doublePawnWeight);
    res -= doublePawn(board, Player::BLACK, doublePawnWeight);
    res += isolatePawn(board, Player::WHITE, isolatedPawnWeight);
    res -= isolatePawn(board, Player::BLACK, isolatedPawnWeight);
    res += passedPawn(board, Player::WHITE, passedPawnWeight, passedPawnRankWeight);
    res -= passedPawn(board, Player::BLACK, passedPawnWeight, passedPawnRankWeight);

    return res;
}