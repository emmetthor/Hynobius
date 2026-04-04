#include "move/Make_BitMove.h"
#include "debug.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"

void doRegularMove(Board& board, const MoveState& state)
{
    board.set(state.from, Piece::EMPTY);
    board.set(state.to, state.placedPiece);
}

void doCastling(Board& board, const MoveState& state)
{
    // move king.
    if (board.at(state.from) != makePiece(board.player, 'K'))
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid castling: not moving a king");
    }

    board.set(state.from, Piece::EMPTY);
    board.set(state.to, state.movePiece);

    Position rookFrom, rookTo;

    // short castle: king e -> g
    if (state.to.col == 6)
    {
        rookFrom = {state.from.row, 7};
        rookTo = {state.from.row, 5};
    }
    // long castle: king e -> c
    else if (state.to.col == 2)
    {
        rookFrom = {state.from.row, 0};
        rookTo = {state.from.row, 3};
    }
    else
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid castling");
    }

    Piece rook = board.at(rookFrom);
    board.set(rookFrom, Piece::EMPTY);
    board.set(rookTo, rook);
}

void undoCastling(Board& board, const UndoState& state)
{
    // move king back
    if (board.at(state.to) != state.movePiece)
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid undo castling: king not at destination");
    }

    board.set(state.to, Piece::EMPTY);
    board.set(state.from, state.movePiece);

    Position rookFrom, rookTo;

    // short castle: rook f -> h
    if (state.to.col == 6)
    {
        rookFrom = {state.from.row, 5};
        rookTo = {state.from.row, 7};
    }
    // long castle: rook d -> a
    else if (state.to.col == 2)
    {
        rookFrom = {state.from.row, 3};
        rookTo = {state.from.row, 0};
    }
    else
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid undo castling");
    }

    Piece rook = board.at(rookFrom);

    if (rook != makePiece(state.player, 'R'))
    {
        ENGINE_FATAL(DebugCategory::MOVE, "invalid undo castling: rook not at expected square");
    }

    board.set(rookFrom, Piece::EMPTY);
    board.set(rookTo, rook);
}

void doBitMove(Board& board, const BitMove move, UndoState& undo)
{
    // make current move state.
    MoveState state(board, move);

    // save info for UndoState
    undo.make(board, state);

    // do move
    if (state.isCastle)
    {
        doCastling(board, state);
    }
    else
    {
        doRegularMove(board, state);
    }

    // update material score.
    board.materialScore = computePieceValue(board);

    // update PST score.
    board.PSTScore = computePST(board);

    // update Zobrist.
    board.zobristKey = computeZobrist(board);

    // update piece pos.
    computePiecePos(board);

    // change player.
    board.player = opponent(board.player);
}

void undoBitMove(Board& board, const BitMove move, const UndoState& undo)
{
    if (undo.isCastle)
    {
        undoCastling(board, undo);
    }
    else
    {
        board.set(undo.to, undo.capturedPiece);
        board.set(undo.from, undo.movePiece);
    }

    board.castleRights = undo.castleRights;
    board.materialScore = undo.materialScore;
    board.PSTScore = undo.PSTScore;
    board.zobristKey = undo.zobristKey;

    board.player = opponent(board.player);

    computePiecePos(board);
}