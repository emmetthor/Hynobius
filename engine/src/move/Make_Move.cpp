#include "move/Make_Move.h"
#include "board/Board.h"
#include "debug.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"
#include <assert.h>

// 入堡走子
void castleMove(Board& board, Move& move)
{
    CastleMove c = getCastleMove(move);
    Player player = move.player;
    int playerScoreControl = (player == Player::WHITE ? 1 : -1);

    board.set(c.kingFrom, Piece::EMPTY);
    board.set(c.kingTo, c.kingPiece);
    board.set(c.rookFrom, Piece::EMPTY);
    board.set(c.rookTo, c.rookPiece);

    board.materialScore -= playerScoreControl * evaluatePieceSquare(c.kingPiece, c.kingFrom);
    board.materialScore += playerScoreControl * evaluatePieceSquare(c.kingPiece, c.kingTo);

    board.materialScore -= playerScoreControl * evaluatePieceSquare(c.rookPiece, c.rookFrom);
    board.materialScore += playerScoreControl * evaluatePieceSquare(c.rookPiece, c.rookTo);

    board.zobristKey ^= zobPiece[pieceToIndex(c.kingPiece)][zobBoardPosition(c.kingFrom)];
    board.zobristKey ^= zobPiece[pieceToIndex(c.kingPiece)][zobBoardPosition(c.kingTo)];
    board.zobristKey ^= zobPiece[pieceToIndex(c.rookPiece)][zobBoardPosition(c.rookFrom)];
    board.zobristKey ^= zobPiece[pieceToIndex(c.rookPiece)][zobBoardPosition(c.rookTo)];

    int kingIndex = pieceToIndex(c.kingPiece);
    int rookIndex = pieceToIndex(c.rookPiece);

    board.piecePosDelete(board.piecePos[kingIndex], board.pieceCount[kingIndex], c.kingFrom);
    board.piecePosAdd(board.piecePos[kingIndex], board.pieceCount[kingIndex], c.kingTo);
    board.piecePosDelete(board.piecePos[rookIndex], board.pieceCount[rookIndex], c.rookFrom);
    board.piecePosAdd(board.piecePos[rookIndex], board.pieceCount[rookIndex], c.rookTo);
}

// 還原入堡
void undoCastleMove(Board& board, Move& move)
{
    CastleMove c = getCastleMove(move);
    Player player = move.player;

    board.set(c.kingFrom, c.kingPiece);
    board.set(c.kingTo, Piece::EMPTY);
    board.set(c.rookFrom, c.rookPiece);
    board.set(c.rookTo, Piece::EMPTY);

    int kingIndex = pieceToIndex(c.kingPiece);
    int rookIndex = pieceToIndex(c.rookPiece);

    board.piecePosDelete(board.piecePos[kingIndex], board.pieceCount[kingIndex], c.kingTo);
    board.piecePosAdd(board.piecePos[kingIndex], board.pieceCount[kingIndex], c.kingFrom);
    board.piecePosDelete(board.piecePos[rookIndex], board.pieceCount[rookIndex], c.rookTo);
    board.piecePosAdd(board.piecePos[rookIndex], board.pieceCount[rookIndex], c.rookFrom);
}

int updateCastleRights(int castleRights, const Move& move)
{
    Player player = move.player;
    int fromCol = move.from.col;

    // 動 king → 清掉該方所有 castle bit
    if (move.movePiece == makePiece(player, 'K'))
    {
        if (player == Player::WHITE)
        {
            castleRights &= ~0b1100; // 清 bit2 bit3 → 白方 king + queen side
        }
        else
        {
            castleRights &= ~0b0011; // 清 bit0 bit1 → 黑方 king + queen side
        }
    }

    // 動 rook → 清掉對應側
    else if (move.movePiece == makePiece(player, 'R'))
    {
        if (player == Player::WHITE)
        {
            if (fromCol == 0)
                castleRights &= ~0b0100; // WQ
            else if (fromCol == 7)
                castleRights &= ~0b1000; // WK
        }
        else
        {
            if (fromCol == 0)
                castleRights &= ~0b0001; // BQ
            else if (fromCol == 7)
                castleRights &= ~0b0010; // BK
        }
    }

    return castleRights;
}

// 執行 move
void makeMove(Board& board, Move& move)
{
    move.prevCastleRights = board.castleRights;
    move.prevMateralPoints = board.materialScore;
    move.prevPST = board.PSTScore;
    move.prevZobrist = board.zobristKey;

    // 控制 castleRights

    board.zobristKey ^= zobCastle[board.castleRights];
    board.castleRights = updateCastleRights(board.castleRights, move);
    board.zobristKey ^= zobCastle[board.castleRights];

    // std::cout << board.castleRights << '\n';

    // 執行 move
    Piece captured = move.capturePiece;
    Piece moved = move.movePiece;
    Player player = move.player;
    int playerScoreControl = (player == Player::WHITE ? 1 : -1);

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        castleMove(board, move);
    }

    else if (move.isPromotion)
    {
        board.set(move.from, Piece::EMPTY);
        board.set(move.to, move.promotionPiece);
    }

    else
    {
        board.set(move.from, Piece::EMPTY);
        board.set(move.to, moved);
    }

    // 更新 material score
    if (captured != Piece::EMPTY)
    {
        board.materialScore += playerScoreControl * pieceValue(captured);
    }

    if (move.isPromotion)
    {
        board.materialScore +=
            playerScoreControl * (pieceValue(move.promotionPiece) - pieceValue(moved));
    }

    // 更新 PST

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // castleMove 完成
    }

    else if (move.isPromotion)
    {
        board.PSTScore -= playerScoreControl * evaluatePieceSquare(moved, move.from);
        board.PSTScore += playerScoreControl * evaluatePieceSquare(move.promotionPiece, move.to);
    }

    else
    {
        board.PSTScore -= playerScoreControl * evaluatePieceSquare(moved, move.from);
        board.PSTScore += playerScoreControl * evaluatePieceSquare(moved, move.to);
    }

    if (captured != Piece::EMPTY)
    {
        board.PSTScore -= -playerScoreControl * evaluatePieceSquare(captured, move.to);
    }

    // 更新Zobrist
    int fromZob = zobBoardPosition(move.from);
    int toZob = zobBoardPosition(move.to);

    board.zobristKey ^= zobPlayer;

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // castleMove 完成
    }

    else if (move.isPromotion)
    {
        board.zobristKey ^= zobPiece[pieceToIndex(moved)][fromZob];
        board.zobristKey ^= zobPiece[pieceToIndex(move.promotionPiece)][toZob];
    }

    else
    {
        board.zobristKey ^= zobPiece[pieceToIndex(moved)][fromZob];
        board.zobristKey ^= zobPiece[pieceToIndex(moved)][toZob];
    }

    if (captured != Piece::EMPTY)
    {
        board.zobristKey ^= zobPiece[pieceToIndex(captured)][toZob];
    }

    // WARN assert(computeZobrist(board, opponent(player)) == board.zobristKey);

    // 更新piecePos

    if (captured != Piece::EMPTY)
    {
        // std::cout << "capture delete\n";
        int captureIndex = pieceToIndex(captured);
        board.piecePosDelete(board.piecePos[captureIndex], board.pieceCount[captureIndex], move.to);
    }

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // castleMove 完成
    }

    else if (move.isPromotion)
    {
        int moveIndex = pieceToIndex(moved);
        int promotionIndex = pieceToIndex(move.promotionPiece);
        board.piecePosDelete(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.from);
        board.piecePosAdd(
            board.piecePos[promotionIndex], board.pieceCount[promotionIndex], move.to);
    }

    else
    {
        int moveIndex = pieceToIndex(moved);
        board.piecePosDelete(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.from);
        board.piecePosAdd(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.to);
    }

    // std::cout << move << '\n';
    ENGINE_ASSERT(validatePiecePos(board));

    board.player = opponent(board.player);
}

void undoMove(Board& board, Move& move)
{
    board.castleRights = move.prevCastleRights;
    board.materialScore = move.prevMateralPoints;
    board.PSTScore = move.prevPST;
    board.zobristKey = move.prevZobrist;

    // 執行 undo
    Piece captured = move.capturePiece;
    Piece moved = move.movePiece;
    Player player = move.player;

    move.capturePiece = captured;

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        undoCastleMove(board, move);
    }

    else if (move.isPromotion)
    {
        board.set(move.from, moved);
        board.set(move.to, captured);
    }

    else
    {
        board.set(move.from, moved);
        board.set(move.to, captured);
    }

    // 回復 piecePos

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // undoCastleMove 完成
    }

    else if (move.isPromotion)
    {
        int moveIndex = pieceToIndex(moved);
        int promotionIndex = pieceToIndex(move.promotionPiece);
        board.piecePosDelete(
            board.piecePos[promotionIndex], board.pieceCount[promotionIndex], move.to);
        board.piecePosAdd(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.from);
    }

    else
    {
        int moveIndex = pieceToIndex(moved);
        board.piecePosDelete(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.to);
        board.piecePosAdd(board.piecePos[moveIndex], board.pieceCount[moveIndex], move.from);
    }

    if (captured != Piece::EMPTY)
    {
        int captureIndex = pieceToIndex(captured);
        board.piecePosAdd(board.piecePos[captureIndex], board.pieceCount[captureIndex], move.to);
    }

    ENGINE_ASSERT(validatePiecePos(board));

    board.player = opponent(board.player);
}