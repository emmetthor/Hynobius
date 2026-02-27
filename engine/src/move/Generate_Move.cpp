#pragma GCC optimize("O3,unroll-loops")

#include "board/Board.h"
#include "board/Piece.h"
#include "move/Move.h"
#include "move/Make_Move.h"
#include "move/Generate_Position.h"
#include "board/Check.h"
#include "board/Attack.h"
#include "pgn/Pgn_Transformer.h"
#include "Structure_IO.h"
#include "debug.h"

#include <vector>

int generatePieceMoves(
    const Board &board,
    Player player,
    Piece movePiece,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;
    int pieceCount = board.getPieceCount(movePiece);
    const auto *posArray = board.getPiecePos(movePiece);

    Position posBuffer[30];
            
    for (int i = 0; i < pieceCount; i++) {
        Position pos = posArray[i];

        int n = generatePiecePosFromPos(board, pos, movePiece, posBuffer);

        for (int i = 0; i < n; i++) {
            buffer[cnt++] = makeBitMove(
                positionToSquare(pos),
                positionToSquare(posBuffer[i]),
                Piece::EMPTY,
                (board.at(posBuffer[i]) == Piece::EMPTY ? false : true),
                false,
                false,
                false
            );
        }
    }

    return cnt;
}

int generatePieceCapture(
    const Board &board,
    Player player,
    Piece movePiece,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;
    int pieceCount = board.getPieceCount(movePiece);
    const auto *posArray = board.getPiecePos(movePiece);
    
    Position posBuffer[30];
            
    for (int i = 0; i < pieceCount; i++) {
        Position pos = posArray[i];

        int n = generatePieceCaptureFromPos(board, pos, movePiece, posBuffer);

        for (int i = 0; i < n; i++) {
            buffer[cnt++] = makeBitMove(
                positionToSquare(pos),
                positionToSquare(posBuffer[i]),
                Piece::EMPTY,
                (board.at(posBuffer[i]) == Piece::EMPTY ? false : true),
                false,
                false,
                false
            );
        }
    }

    return cnt;
}

int generateAllMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));

    int cnt = 0;
    Piece 
        pawn    = makePiece(player, 'P'),
        knight  = makePiece(player, 'N'),
        bishop  = makePiece(player, 'B'),
        rook    = makePiece(player, 'R'),
        queen   = makePiece(player, 'Q'),
        king    = makePiece(player, 'K');

    cnt += generatePieceMoves(board, player, knight, buffer);
    cnt += generatePieceMoves(board, player, bishop, buffer + cnt);
    cnt += generatePieceMoves(board, player, rook, buffer + cnt);
    cnt += generatePieceMoves(board, player, queen, buffer + cnt);
    cnt += generatePieceMoves(board, player, king, buffer + cnt);

    // pawn
    int dr = (player == Player::WHITE ? -1 : 1);
    int startRank = (player == Player::WHITE ? 6 : 1);
    int promoteRank = (player == Player::WHITE ? 0 : 7);
    int pawnCount = board.getPieceCount(pawn);
    const auto *posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++) {
        Position from = posArray[i];
        auto [r, c] = posArray[i];
         Position to = {r + dr, c};

        // 一步
        if (isInBoard(to) && board.at(to) == Piece::EMPTY) {
            if (r + dr == promoteRank) {
                for (auto promo : {knight, bishop, rook, queen}) {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(from),
                        positionToSquare(to),
                        promo,
                        false,
                        false,
                        false,
                        true
                    );
                }
            } else {
                buffer[cnt++] = makeBitMove(
                    positionToSquare(from),
                    positionToSquare(to),
                    Piece::EMPTY,
                    false,
                    false,
                    false,
                    false
                );
            }
        }

        // 兩步
        if (r == startRank) {
            Position mid = {r + dr, c};
            Position toTwoStep = {r + 2 * dr, c};

            if (isInBoard(toTwoStep) && board.at(toTwoStep) == Piece::EMPTY && board.at(mid) == Piece::EMPTY) {
                buffer[cnt++] = makeBitMove(
                    positionToSquare(from),
                    positionToSquare(toTwoStep),
                    Piece::EMPTY,
                    false,
                    false,
                    false,
                    false
                );
            }
        }

        // capture
        for (int dc : {-1, 1}) {
            Position toCapture = {r + dr, c + dc};
            if (!isInBoard(toCapture)) continue;
            if (board.at(toCapture) == Piece::EMPTY) continue;
            if (isSameColor(board.at(toCapture), pawn)) continue;

            if (toCapture.row == promoteRank) {
                for (auto promo : {knight, bishop, rook, queen}) {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(from),
                        positionToSquare(toCapture),
                        promo,
                        true,
                        false,
                        false,
                        true
                    );
                }
            } else {
                buffer[cnt++] = makeBitMove(
                    positionToSquare(from),
                    positionToSquare(toCapture),
                    Piece::EMPTY,
                    true,
                    false,
                    false,
                    false
                );
            }
        }
    }

    // castle
    //WARN 目前所有 castle move 皆不會生成
    // for (auto len: {SHORT_CASTLE, LONG_CASTLE}) {
    //     Move moveCastle;
    //     moveCastle.castle = len;
    //     moveCastle.player = player;
    //     moveCastle.capturePiece = Piece::EMPTY;

    //     CastleMove m = getCastleMove(moveCastle);
    //     moveCastle.from = m.kingFrom;
    //     moveCastle.to = m.kingTo;

    //     buffer[cnt++] = moveCastle;
    // }

    return cnt;
}

int generateCaptureMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));
    ENGINE_ASSERT(validatePiecePos(board));
    
    int cnt = 0;
    Piece 
        pawn =      makePiece(player, 'P'),
        knight =    makePiece(player, 'N'),
        bishop =    makePiece(player, 'B'),
        rook =      makePiece(player, 'R'),
        queen =     makePiece(player, 'Q'),
        king =      makePiece(player, 'K');

    cnt += generatePieceCapture(board, player, knight, buffer);
    cnt += generatePieceCapture(board, player, bishop, buffer + cnt);
    cnt += generatePieceCapture(board, player, rook, buffer + cnt);
    cnt += generatePieceCapture(board, player, queen, buffer + cnt);
    cnt += generatePieceCapture(board, player, king, buffer + cnt);

    // pawn
    int dr = (player == Player::WHITE ? -1 : 1);
    int startRank = (player == Player::WHITE ? 6 : 1);
    int promoteRank = (player == Player::WHITE ? 0 : 7);
    int pawnCount = board.getPieceCount(pawn);
    const auto *posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++) {
        Position from = posArray[i];
        auto [r, c] = posArray[i];
        Position to = {r + dr, c};

        // capture
        for (int dc : {-1, 1}) {
            Position toCapture = {r + dr, c + dc};
            if (!isInBoard(toCapture)) continue;
            if (board.at(toCapture) == Piece::EMPTY) continue;
            if (isSameColor(board.at(toCapture), pawn)) continue;

            if (toCapture.row == promoteRank) {
                for (auto promo : {knight, bishop, rook, queen}) {
                    buffer[cnt++] = makeBitMove(
                        positionToSquare(from),
                        positionToSquare(toCapture),
                        promo,
                        true,
                        false,
                        false,
                        true
                    );
                }
            } else {
                buffer[cnt++] = makeBitMove(
                    positionToSquare(from),
                    positionToSquare(toCapture),
                    Piece::EMPTY,
                    true,
                    false,
                    false,
                    false
                );
            }
        }
    }

    return cnt;
}

int filterLegalMoves(
    const Board &board,
    const Player player,
    BitMove *allMoves,
    int nAllMoves,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));

    int cnt = 0;
    Board copyBoard = board;

    ENGINE_ASSERT(validatePiecePos(board));

    for (int i = 0; i < nAllMoves; i++) {
        BitMove &move = allMoves[i];
        //if (!isMoveLegal(board, move)) continue; 已經是正確的

        if (getCastle(move)) {
            // WARN 目前castle 全部停擺
            // if (!isCastleLegal(board, move)) continue;
        }

        Move transed = bitMovetoOriMove(copyBoard, move);

        makeMove(copyBoard, transed);

        if (!isInCheck(copyBoard, player)) {
            buffer[cnt++] = move;
        }

        undoMove(copyBoard, transed);
    }

    return cnt;
}

int generateAllLegalMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));

    //std::cout << "generate board:\n" << board << '\n';

    BitMove allMoves[2000];
    int nAll = generateAllMoves(board, player, allMoves);

    int nLegalMoves = filterLegalMoves(board, player, allMoves, nAll, buffer);

    return nLegalMoves;
}

int generateLegalCaptureMoves(
    const Board &board,
    const Player player,
    BitMove *buffer
) {
    ENGINE_ASSERT(isPlayerValid(player));
    
    BitMove captureMoves[2000];
    int ncaptureMoves = generateCaptureMoves(board, player, captureMoves);

    int nLegalMoves = filterLegalMoves(board, player, captureMoves, ncaptureMoves, buffer);

    return nLegalMoves;
}