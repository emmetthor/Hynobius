#include "move/Generate_Position.h"
#include "board/Board.h"
#include "debug.h"

// WARN 這是第二次出現，之後要與 Attack 整合
// 棋子移動的固定 map
// jump piece
static const int KNIGHT_DR[] = {1, 2, 2, 1, -1, -2, -2, -1};
static const int KNIGHT_DC[] = {2, 1, -1, -2, -2, -1, 1, 2};
static const int KING_DR[] = {1, 1, 1, 0, -1, -1, -1, 0};
static const int KING_DC[] = {1, 0, -1, -1, -1, 0, 1, 1};
// slide piece
static const int ROOK_QUEEN_DR[] = {1, -1, 0, 0};
static const int ROOK_QUEEN_DC[] = {0, 0, 1, -1};
static const int BISHOP_QUEEN_DR[] = {1, 1, -1, -1};
static const int BISHOP_QUEEN_DC[] = {1, -1, 1, -1};

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
            return generatePosFromPosWithJumpPiece(board, pos, p, KNIGHT_DR, KNIGHT_DC, buffer);

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            return generatePosFromPosWithSlidePiece(
                board, pos, p, BISHOP_QUEEN_DR, BISHOP_QUEEN_DC, buffer);

        case Piece::WROOK:
        case Piece::BROOK:
            return generatePosFromPosWithSlidePiece(
                board, pos, p, ROOK_QUEEN_DR, ROOK_QUEEN_DC, buffer);

        case Piece::WQUEEN:
        case Piece::BQUEEN:
        {
            int n1 = generatePosFromPosWithSlidePiece(
                board, pos, p, ROOK_QUEEN_DR, ROOK_QUEEN_DC, buffer);
            int n2 = generatePosFromPosWithSlidePiece(
                board, pos, p, BISHOP_QUEEN_DR, BISHOP_QUEEN_DC, buffer + n1);
            return n1 + n2;
        }

        case Piece::WKING:
        case Piece::BKING:
            return generatePosFromPosWithJumpPiece(board, pos, p, KING_DR, KING_DC, buffer);

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL(DebugCategory::GENERATE, "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL(DebugCategory::GENERATE, "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL(DebugCategory::GENERATE, "piece_count is not a piece.");
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
            return generateCaptureFromPosWithJumpPiece(board, pos, p, KNIGHT_DR, KNIGHT_DC, buffer);

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            return generateCaptureFromPosWithSlidePiece(
                board, pos, p, BISHOP_QUEEN_DR, ROOK_QUEEN_DC, buffer);

        case Piece::WROOK:
        case Piece::BROOK:
            return generateCaptureFromPosWithSlidePiece(
                board, pos, p, ROOK_QUEEN_DR, ROOK_QUEEN_DC, buffer);

        case Piece::WQUEEN:
        case Piece::BQUEEN:
        {
            int n1 = generateCaptureFromPosWithSlidePiece(
                board, pos, p, ROOK_QUEEN_DR, ROOK_QUEEN_DC, buffer);
            int n2 = generateCaptureFromPosWithSlidePiece(
                board, pos, p, BISHOP_QUEEN_DR, ROOK_QUEEN_DC, buffer + n1);
            return n1 + n2;
        }

        case Piece::WKING:
        case Piece::BKING:
            return generateCaptureFromPosWithJumpPiece(board, pos, p, KING_DR, KING_DC, buffer);

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL(DebugCategory::GENERATE, "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL(DebugCategory::GENERATE, "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL(DebugCategory::GENERATE, "piece_count is not a piece.");
    }

    return 0;
}