#include "Engine.h"

#include "board/Board.h"
#include "board/Piece.h"
#include "fen/FEN_Parser.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "search/Search.h"

Engine::Engine() {}

void Engine::setStartPosition()
{
    board.init();
}

void Engine::setPositionWithFen(const std::string& fen)
{
    board = cinFenToBoard(fen);
}

void Engine::checkReady() {}

Board Engine::getBoard()
{
    return board;
}

void Engine::move(Move move)
{
    BitMove bitMove =
        makeBitMove(positionToSquare(move.from),
                    positionToSquare(move.to),
                    move.promotionPiece,
                    (isValidPieceIndex(pieceToIndex(move.capturePiece)) ? true : false),
                    (move.castle == Castle::NOT ? false : true),
                    false,
                    move.isPromotion);

    UndoState undo;
    doBitMove(board, bitMove, undo);
}

void Engine::changePlayer()
{
    board.player = opponent(board.player);
}

void Engine::setPlayer(Player player)
{
    board.player = player;
}

Move Engine::goDepth(int depth, bool isPrintInfo)
{
    Search search(eval);
    auto res = search.findBestMove(board, depth);

    if (isPrintInfo)
    {
        printInfo(res.info);
    }

    return res.bestMove;
}

SearchResult Engine::fullInfoSearch(int depth)
{
    Search search(eval);
    return search.findBestMove(board, depth);
}

void Engine::quit() {}