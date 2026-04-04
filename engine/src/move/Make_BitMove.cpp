#include "move/Make_BitMove.h"
#include "Structure_IO.h"
#include "board/Board.h"
#include "board/Piece.h"
#include "debug.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"

void doRegularMove(Board& board, const MoveState& state)
{
    board.set(state.from, Piece::EMPTY);
    board.set(state.to, state.placedPiece);
}

// update castle rights.
// castle bit stores:
// - bit0 black king side
// - bit1 black queen side
// - bit2 white king side
// - bit3 white queen side
int updateCastleRights(int castleRights, const MoveState& state)
{
    Player player = state.player;
    int fromCol = state.from.col;

    // move king -> remove every castle rights.
    if (state.movePiece == makePiece(player, 'K'))
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
    else if (state.movePiece == makePiece(player, 'R'))
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
    else
    {
        if (state.isCapture && state.capturedPiece == makePiece(Player::WHITE, 'R'))
        {
            if (state.to.row == 0 && state.to.col == 0) castleRights &= ~0b1000;
            if (state.to.row == 0 && state.to.col == 7) castleRights &= ~0b0100;
        }
        if (state.isCapture && state.capturedPiece == makePiece(Player::BLACK, 'R'))
        {
            if (state.to.row == 7 && state.to.col == 0) castleRights &= ~0b0010;
            if (state.to.row == 7 && state.to.col == 7) castleRights &= ~0b0001;
        }
    }

    return castleRights;
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
        std::cout << board << '\n';
        ENGINE_FATAL(DebugCategory::MOVE, "invalid undo castling: rook not at expected square");
    }

    board.set(rookFrom, Piece::EMPTY);
    board.set(rookTo, rook);
}

void updateMaterialScoreDo(Board& board, const MoveState& state, int weight)
{
    if (state.isCastle)
    {
        // castling should not change material score.
        return;
    }

    if (state.isCapture)
    {
        // capture enemy's -> get the piece's score
        board.materialScore += weight * pieceValue(state.capturedPiece);
    }

    if (state.isPromotion)
    {
        board.materialScore -= weight * pieceValue(state.movePiece);
        board.materialScore += weight * pieceValue(state.placedPiece);
    }
}

void updatePSTScoreDo(Board& board, const MoveState& state, int weight)
{
    // moving piece leaves from
    board.PSTScore -= weight * evaluatePieceSquare(state.movePiece, state.from);

    // placed piece arrives at to
    board.PSTScore += weight * evaluatePieceSquare(state.placedPiece, state.to);

    // captured piece disappears
    if (state.isCapture)
    {
        Position capturedPos = state.to; // normal capture

        // capture enemy's -> weight *= -1
        board.PSTScore -= -weight * evaluatePieceSquare(state.capturedPiece, capturedPos);
    }

    // castling
    if (state.isCastle)
    {
        Position rookFrom, rookTo;

        if (state.to.col == 6) // short castle
        {
            rookFrom = {state.from.row, 7};
            rookTo = {state.from.row, 5};
        }
        else if (state.to.col == 2) // long castle
        {
            rookFrom = {state.from.row, 0};
            rookTo = {state.from.row, 3};
        }
        else
        {
            ENGINE_FATAL(DebugCategory::MOVE, "invalid castling in PST update");
        }

        Piece rook = makePiece(state.player, 'R');

        board.PSTScore -= weight * evaluatePieceSquare(rook, rookFrom);
        board.PSTScore += weight * evaluatePieceSquare(rook, rookTo);
    }
}

void updateZobristDo(Board& board, const MoveState& state, int oldCastleRights, int newCastleRights)
{
    int movedPieceIndex = pieceToIndex(state.movePiece);
    int capturedPieceIndex = pieceToIndex(state.capturedPiece);
    int placedPieceIndex = pieceToIndex(state.placedPiece);

    // moving piece leaves from
    board.zobristKey ^= zobPiece[movedPieceIndex][zobBoardPosition(state.from)];

    // placed piece arrives at to
    board.zobristKey ^= zobPiece[placedPieceIndex][zobBoardPosition(state.to)];

    // captured piece disappears
    if (state.isCapture)
    {
        board.zobristKey ^= zobPiece[capturedPieceIndex][zobBoardPosition(state.to)];
    }

    // castling
    if (state.isCastle)
    {
        Position rookFrom, rookTo;

        if (state.to.col == 6) // short castle
        {
            rookFrom = {state.from.row, 7};
            rookTo = {state.from.row, 5};
        }
        else if (state.to.col == 2) // long castle
        {
            rookFrom = {state.from.row, 0};
            rookTo = {state.from.row, 3};
        }
        else
        {
            ENGINE_FATAL(DebugCategory::MOVE, "invalid castling in PST update");
        }

        int rookIndex = pieceToIndex(makePiece(state.player, 'R'));

        board.zobristKey ^= zobPiece[rookIndex][zobBoardPosition(rookFrom)];
        board.zobristKey ^= zobPiece[rookIndex][zobBoardPosition(rookTo)];
    }

    board.zobristKey ^= zobPlayer;

    // update castle right zob
    board.zobristKey ^= zobCastle[oldCastleRights];
    board.zobristKey ^= zobCastle[newCastleRights];
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

    // update castle rights.
    int oldCastleRights = state.castleRights;
    int newCastelRights = updateCastleRights(oldCastleRights, state);
    board.castleRights = newCastelRights;

    // calculate weight
    int weight = (state.player == Player::WHITE ? 1 : -1);

    // update material score.
    updateMaterialScoreDo(board, state, weight);

    // update PST score.
    updatePSTScoreDo(board, state, weight);

    // update piece pos.
    computePiecePos(board);

    // change player.
    board.player = opponent(board.player);

    // update Zobrist.
    updateZobristDo(board, state, oldCastleRights, newCastelRights);

    assert(board.materialScore == computePieceValue(board));
    assert(board.PSTScore == computePST(board));
    assert(board.zobristKey == computeZobrist(board));
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

    assert(board.materialScore == computePieceValue(board));
    assert(board.PSTScore == computePST(board));
    assert(board.zobristKey == computeZobrist(board));
}