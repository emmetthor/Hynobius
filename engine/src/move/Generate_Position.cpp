#include "move/Generate_Position.h"
#include "board/Board.h"
#include "debug.h"
#include "move/Move_Direction.h"

int generatePosFromPosWithJumpPiece(const Board& board,
                                    const Position& pos,
                                    Piece movePiece,
                                    const int dr[],
                                    const int dc[],
                                    Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;

    for (int i = 0; i < 8; i++)
    {
        Position p = {pos.row + dr[i], pos.col + dc[i]};
        if (!isInBoard(p))
            continue;

        Piece atPiece = board.at(p);
        if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
            continue;

        buffer[cnt++] = p;
    }

    return cnt;
}

int generatePosFromPosWithSlidePiece(const Board& board,
                                     const Position& pos,
                                     Piece movePiece,
                                     const int dr[],
                                     const int dc[],
                                     Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;

    for (int i = 0; i < 4; i++)
    {
        Position p = {pos.row + dr[i], pos.col + dc[i]};

        while (true)
        {
            if (!isInBoard(p))
                break;
            Piece atPiece = board.at(p);
            if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
                break;

            buffer[cnt++] = p;

            if (atPiece != Piece::EMPTY)
                break;

            p.row += dr[i];
            p.col += dc[i];
        }
    }

    return cnt;
}

int generateCaptureFromPosWithJumpPiece(const Board& board,
                                        const Position& pos,
                                        Piece movePiece,
                                        const int dr[],
                                        const int dc[],
                                        Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;

    for (int i = 0; i < 8; i++)
    {
        Position p = {pos.row + dr[i], pos.col + dc[i]};
        if (!isInBoard(p))
            continue;

        Piece atPiece = board.at(p);
        if (atPiece == Piece::EMPTY)
            continue;
        if (isSameColor(atPiece, movePiece))
            continue;

        buffer[cnt++] = p;
    }

    return cnt;
}

int generateCaptureFromPosWithSlidePiece(const Board& board,
                                         const Position& pos,
                                         Piece movePiece,
                                         const int dr[],
                                         const int dc[],
                                         Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;

    for (int i = 0; i < 4; i++)
    {
        Position p = {pos.row + dr[i], pos.col + dc[i]};

        while (true)
        {
            if (!isInBoard(p))
                break;
            Piece atPiece = board.at(p);

            if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
                break;

            if (atPiece != Piece::EMPTY)
            {
                buffer[cnt++] = p;
                break;
            }

            p.row += dr[i];
            p.col += dc[i];
        }
    }

    return cnt;
}

int generatePiecePosFromPos(const Board& board, const Position& pos, Piece p, Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));

    switch (p)
    {
        case Piece::WKNIGHT:
        case Piece::BKNIGHT:
            return generatePosFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KNIGHT_DR, MoveDirection::KNIGHT_DC, buffer);

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            return generatePosFromPosWithSlidePiece(board,
                                                    pos,
                                                    p,
                                                    MoveDirection::BISHOP_QUEEN_DR,
                                                    MoveDirection::BISHOP_QUEEN_DC,
                                                    buffer);

        case Piece::WROOK:
        case Piece::BROOK:
            return generatePosFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, buffer);

        case Piece::WQUEEN:
        case Piece::BQUEEN:
        {
            int n1 = generatePosFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, buffer);
            int n2 = generatePosFromPosWithSlidePiece(board,
                                                      pos,
                                                      p,
                                                      MoveDirection::BISHOP_QUEEN_DR,
                                                      MoveDirection::BISHOP_QUEEN_DC,
                                                      buffer + n1);
            return n1 + n2;
        }

        case Piece::WKING:
        case Piece::BKING:
            return generatePosFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KING_DR, MoveDirection::KING_DC, buffer);

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL("generate position", "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL("generate position", "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL("generate position", "piece_count is not a piece.");
    }

    return 0;
}

int generatePieceCaptureFromPos(const Board& board, const Position& pos, Piece p, Position* buffer)
{
    ENGINE_ASSERT(isInBoard(pos));

    switch (p)
    {
        case Piece::WKNIGHT:
        case Piece::BKNIGHT:
            return generateCaptureFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KNIGHT_DR, MoveDirection::KNIGHT_DC, buffer);

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            return generateCaptureFromPosWithSlidePiece(board,
                                                        pos,
                                                        p,
                                                        MoveDirection::BISHOP_QUEEN_DR,
                                                        MoveDirection::BISHOP_QUEEN_DC,
                                                        buffer);

        case Piece::WROOK:
        case Piece::BROOK:
            return generateCaptureFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, buffer);

        case Piece::WQUEEN:
        case Piece::BQUEEN:
        {
            int n1 = generateCaptureFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, buffer);
            int n2 = generateCaptureFromPosWithSlidePiece(board,
                                                          pos,
                                                          p,
                                                          MoveDirection::BISHOP_QUEEN_DR,
                                                          MoveDirection::BISHOP_QUEEN_DC,
                                                          buffer + n1);
            return n1 + n2;
        }

        case Piece::WKING:
        case Piece::BKING:
            return generateCaptureFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KING_DR, MoveDirection::KING_DC, buffer);

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL("generate position", "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL("generate position", "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL("generate position", "piece_count is not a piece.");
    }

    return 0;
}