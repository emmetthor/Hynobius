#include "move/Make_Move.h"
#include "board/Board.h"
#include "debug.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"
#include <assert.h>

// make a castle move.
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

// undo a castle move.
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

// update castle rights.
// castle bit stores:
// - bit0 black king side
// - bit1 black queen side
// - bit2 white king side
// - bit3 white queen side
int updateCastleRights(int castleRights, const Move& move)
{
    Player player = move.player;
    int fromCol = move.from.col;

    // move king -> remove every castle rights.
    if (move.movePiece == makePiece(player, 'K'))
    {
        if (player == Player::WHITE)
        {
            // remove bit2 and bit3 -> white king and queen side.
            castleRights &= ~0b1100;
        }
        else
        {
            // remove bit0 and bit1 -> white king and queen side.
            castleRights &= ~0b0011;
        }
    }

    // move rook -> remove the side the rook moved from.
    else if (move.movePiece == makePiece(player, 'R'))
    {
        if (player == Player::WHITE)
        {
            if (fromCol == 0)
                // white queen side
                castleRights &= ~0b1000;
            else if (fromCol == 7)
                // white king side
                castleRights &= ~0b0100;
        }
        else
        {
            if (fromCol == 0)
                // black queen side
                castleRights &= ~0b0010;
            else if (fromCol == 7)
                // black king side
                castleRights &= ~0b0001;
        }
    }

    return castleRights;
}

void makeMove(Board& board, Move& move)
{
    move.prevCastleRights = board.castleRights;
    move.prevMateralPoints = board.materialScore;
    move.prevPST = board.PSTScore;
    move.prevZobrist = board.zobristKey;

    // control castleRights
    board.zobristKey ^= zobCastle[board.castleRights];
    board.castleRights = updateCastleRights(board.castleRights, move);
    board.zobristKey ^= zobCastle[board.castleRights];

    // std::cout << board.castleRights << '\n';

    // make move.
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

    // update material score.
    if (captured != Piece::EMPTY)
    {
        board.materialScore += playerScoreControl * pieceValue(captured);
    }

    if (move.isPromotion)
    {
        board.materialScore +=
            playerScoreControl * (pieceValue(move.promotionPiece) - pieceValue(moved));
    }

    // update PST
    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // finished by castleMove.
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

    // update Zobrist.
    int fromZob = zobBoardPosition(move.from);
    int toZob = zobBoardPosition(move.to);

    board.zobristKey ^= zobPlayer;

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // finished by castleMove.
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

    // update piecePos.
    if (captured != Piece::EMPTY)
    {
        // std::cout << "capture delete\n";
        int captureIndex = pieceToIndex(captured);
        board.piecePosDelete(board.piecePos[captureIndex], board.pieceCount[captureIndex], move.to);
    }

    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // finished by castleMove.
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

    // undo move.
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

    // recover piecePos.
    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        // finished by undoCastleMove.
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