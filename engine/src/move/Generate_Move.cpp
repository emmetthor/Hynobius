#pragma GCC optimize("O3,unroll-loops")

#include "Type.h"
#include "move/Make_BitMove.h"
#include "board/Board.h"
#include "board/Check.h"
#include "board/Piece.h"
#include "debug.h"
#include "move/Generate_Position.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "board/Attack.h"

int generatePieceMoves(const Board& board, Piece movePiece, BitMove* buffer)
{
    // check inputs.
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;
    int pieceCount = board.getPieceCount(movePiece);
    const auto* posArray = board.getPiecePos(movePiece);

    // store piece positions.
    Position posBuffer[30];

    for (int i = 0; i < pieceCount; i++)
    {
        Position fromPos = posArray[i];

        int n = generatePiecePosFromPos(board, fromPos, movePiece, posBuffer);

        // for all moves in posArray.
        for (int i = 0; i < n; i++)
        {
            Position toPos = posBuffer[i];
            Piece capture = board.at(toPos);
            bool isCapture = (isValidPieceIndex(pieceToIndex(capture)) ? 1 : 0);

            buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                        positionToSquare(toPos),
                                        Piece::EMPTY,
                                        isCapture,
                                        false,
                                        false,
                                        false);
        }
    }

    return cnt;
}

int generatePieceCapture(const Board& board, Piece movePiece, BitMove* buffer)
{
    // check inputs.
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;
    int pieceCount = board.getPieceCount(movePiece);
    const auto* posArray = board.getPiecePos(movePiece);

    // store piece positions.
    Position posBuffer[30];

    for (int i = 0; i < pieceCount; i++)
    {
        Position fromPos = posArray[i];

        int n = generatePieceCaptureFromPos(board, fromPos, movePiece, posBuffer);

        // for all captures in posArray.
        for (int i = 0; i < n; i++)
        {
            Position toPos = posBuffer[i];
            Piece capture = board.at(toPos);
            bool isCapture = (isValidPieceIndex(pieceToIndex(capture)) ? 1 : 0);

            if (isCapture == 0)
            {
                ENGINE_FATAL(DebugCategory::MOVE, "generated non-capture moves");
            }

            buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                        positionToSquare(toPos),
                                        Piece::EMPTY,
                                        isCapture,
                                        false,
                                        false,
                                        false);
        }
    }

    return cnt;
}

int generatePawnQuietMoves(const Board& board, BitMove* buffer)
{
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));

    int cnt = 0;
    Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
          bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
          queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    int dr = (player == Player::WHITE ? -1 : 1);
    int startRank = (player == Player::WHITE ? 6 : 1);
    int promoteRank = (player == Player::WHITE ? 0 : 7);
    int pawnCount = board.getPieceCount(pawn);
    const auto* posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++)
    {
        auto [r, c] = posArray[i];
        Position fromPos = posArray[i];
        Position pushOneStep = {r + dr, c};
        Position pushTwoStep = {r + 2 * dr, c};

        if (isInBoard(pushOneStep) && board.at(pushOneStep) == Piece::EMPTY)
        {
            if (pushOneStep.row == promoteRank)
            {
                for (auto promo : {knight, bishop, rook, queen})
                {
                    buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                                positionToSquare(pushOneStep),
                                                promo,
                                                false,
                                                false,
                                                false,
                                                true);
                }
            }
            else
            {
                buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                            positionToSquare(pushOneStep),
                                            Piece::EMPTY,
                                            false,
                                            false,
                                            false,
                                            false);
            }
        }

        if (r == startRank && board.at(pushOneStep) == Piece::EMPTY &&
            board.at(pushTwoStep) == Piece::EMPTY)
        {
            buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                        positionToSquare(pushTwoStep),
                                        Piece::EMPTY,
                                        false,
                                        false,
                                        false,
                                        false);
        }
    }

    return cnt;
}

int generatePawnCaptures(const Board& board, BitMove* buffer)
{
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));

    int cnt = 0;
    Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
          bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
          queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    int dr = (player == Player::WHITE ? -1 : 1);
    int startRank = (player == Player::WHITE ? 6 : 1);
    int promoteRank = (player == Player::WHITE ? 0 : 7);
    int pawnCount = board.getPieceCount(pawn);
    const auto* posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++)
    {
        auto [r, c] = posArray[i];
        Position fromPos = posArray[i];

        for (auto dc : {-1, 1})
        {
            Position toPos = {r + dr, c + dc};
            if (!isInBoard(toPos))
                continue;
            if (board.at(toPos) == Piece::EMPTY)
                continue;
            if (isSameColor(board.at(toPos), pawn))
                continue;

            if (toPos.row == promoteRank)
            {
                for (auto promo : {knight, bishop, rook, queen})
                {
                    buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                                positionToSquare(toPos),
                                                promo,
                                                false,
                                                false,
                                                false,
                                                true);
                }
            }
            else
            {
                buffer[cnt++] = makeBitMove(positionToSquare(fromPos),
                                            positionToSquare(toPos),
                                            Piece::EMPTY,
                                            false,
                                            false,
                                            false,
                                            false);
            }
        }
    }

    return cnt;
}

int generateCastling(const Board& board, BitMove* buffer)
{
    int cnt = 0;
    Player player = board.player;

    int row = (player == Player::WHITE ? 7 : 0);

    Position b = {row, 1};
    Position c = {row, 2};
    Position d = {row, 3};
    Position kingPos = {row, 4};
    Position f = {row, 5};
    Position g = {row, 6};

    // safety: king must be on e-file.
    if (board.at(kingPos) != makePiece(player, 'K'))
    {
        return 0;
    }

    // king side.
    if (player == Player::WHITE)
    {
        // white king side.
        if (board.castleRights & 0b0100)
        {
            if (board.at(f) == Piece::EMPTY &&
                board.at(g) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::BLACK) &&
                    !isSquareAttacked(board, f, Player::BLACK) &&
                    !isSquareAttacked(board, g, Player::BLACK))
                {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(kingPos),
                        positionToSquare(g),
                        Piece::EMPTY,
                        false,
                        true,
                        false,
                        false
                    );
                }
            }
        }
    }
    else
    {
        // black king side.
        if (board.castleRights & 0b0001)
        {
            Position f = {row, 5};
            Position g = {row, 6};

            if (board.at(f) == Piece::EMPTY &&
                board.at(g) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::WHITE) &&
                    !isSquareAttacked(board, f, Player::WHITE) &&
                    !isSquareAttacked(board, g, Player::WHITE))
                {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(kingPos),
                        positionToSquare(g),
                        Piece::EMPTY,
                        false,
                        true,
                        false,
                        false
                    );
                }
            }
        }
    }

    // queen side
    if (player == Player::WHITE)
    {
        // white queen side.
        if (board.castleRights & 0b1000)
        {

            if (board.at(d) == Piece::EMPTY &&
                board.at(c) == Piece::EMPTY &&
                board.at(b) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::BLACK) &&
                    !isSquareAttacked(board, d, Player::BLACK) &&
                    !isSquareAttacked(board, c, Player::BLACK))
                {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(kingPos),
                        positionToSquare(c),
                        Piece::EMPTY,
                        false,
                        true,
                        false,
                        false
                    );
                }
            }
        }
    }
    else
    {
        // black queen side.
        if (board.castleRights & 0b0010)
        {
            if (board.at(d) == Piece::EMPTY &&
                board.at(c) == Piece::EMPTY &&
                board.at(b) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::WHITE) &&
                    !isSquareAttacked(board, d, Player::WHITE) &&
                    !isSquareAttacked(board, c, Player::WHITE))
                {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(kingPos),
                        positionToSquare(c),
                        Piece::EMPTY,
                        false,
                        true,
                        false,
                        false
                    );
                }
            }
        }
    }
    return cnt;
}

int generateAllMoves(const Board& board, BitMove* buffer)
{
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));

    int cnt = 0;
    Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
          bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
          queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    cnt += generatePieceMoves(board, knight, buffer);
    cnt += generatePieceMoves(board, bishop, buffer + cnt);
    cnt += generatePieceMoves(board, rook, buffer + cnt);
    cnt += generatePieceMoves(board, queen, buffer + cnt);
    cnt += generatePieceMoves(board, king, buffer + cnt);

    cnt += generatePawnQuietMoves(board, buffer + cnt);
    cnt += generatePawnCaptures(board, buffer + cnt);

    cnt += generateCastling(board, buffer + cnt);

    return cnt;
}

int generateCaptureMoves(const Board& board, BitMove* buffer)
{
    Player player = board.player;
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));

    int cnt = 0;
    Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
          bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
          queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    cnt += generatePieceCapture(board, knight, buffer);
    cnt += generatePieceCapture(board, bishop, buffer + cnt);
    cnt += generatePieceCapture(board, rook, buffer + cnt);
    cnt += generatePieceCapture(board, queen, buffer + cnt);
    cnt += generatePieceCapture(board, king, buffer + cnt);

    cnt += generatePawnCaptures(board, buffer + cnt);

    return cnt;
}

int filterLegalMoves(const Board& board, BitMove* allMoves, int nAllMoves, BitMove* buffer)
{
    ENGINE_ASSERT(isPlayerValid(player));

    int cnt = 0;
    Board copyBoard = board;

    ENGINE_ASSERT(validatePiecePos(board));

    for (int i = 0; i < nAllMoves; i++)
    {
        BitMove move = allMoves[i];
        UndoState undo;

        doBitMove(copyBoard, move, undo);

        if (!isInCheck(copyBoard, opponent(copyBoard.player)))
        {
            buffer[cnt++] = move;
        }

        undoBitMove(copyBoard, move, undo);
    }

    return cnt;
}

int generateAllLegalMoves(const Board& board, Move* buffer)
{
    ENGINE_ASSERT(isPlayerValid(board.player));

    BitMove allMoves[2000];
    int nAll = generateAllMoves(board, allMoves);

    BitMove filterdMoves[2000];
    int nLegalMoves = filterLegalMoves(board, allMoves, nAll, filterdMoves);

    for (int i = 0; i < nLegalMoves; i++) buffer[i] = bitMovetoOriMove(board, filterdMoves[i]);

    return nLegalMoves;
}

int generateAllLegalMoves(const Board& board, BitMove* buffer)
{
    ENGINE_ASSERT(isPlayerValid(board.player));

    BitMove allMoves[2000];
    int nAll = generateAllMoves(board, allMoves);

    int nLegalMoves = filterLegalMoves(board, allMoves, nAll, buffer);

    return nLegalMoves;
}

int generateLegalCaptureMoves(const Board& board, Move* buffer)
{
    ENGINE_ASSERT(isPlayerValid(player));

    BitMove captureMoves[2000];
    int ncaptureMoves = generateCaptureMoves(board, captureMoves);

    BitMove filterdMoves[2000];
    int nLegalMoves = filterLegalMoves(board, captureMoves, ncaptureMoves, filterdMoves);

    for (int i = 0; i < nLegalMoves; i++) buffer[i] = bitMovetoOriMove(board, filterdMoves[i]);

    return nLegalMoves;
}

int generateLegalCaptureMoves(const Board& board, BitMove* buffer)
{
    ENGINE_ASSERT(isPlayerValid(player));

    BitMove captureMoves[2000];
    int ncaptureMoves = generateCaptureMoves(board, captureMoves);

    int nLegalMoves = filterLegalMoves(board, captureMoves, ncaptureMoves, buffer);

    return nLegalMoves;
}