#include "board/Board.h"
#include "Structure_IO.h"
#include "board/Piece.h"
#include "debug/log.h"
#include "search/Zobrist.h"

Board::Board()
{
    init();
}

void Board::init()
{
    player = Player::WHITE;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            board[r][c] = Piece::EMPTY;
        }
    }

    // 黑方棋子
    board[0][0] = board[0][7] = Piece::BROOK;
    board[0][1] = board[0][6] = Piece::BKNIGHT;
    board[0][2] = board[0][5] = Piece::BBISHOP;
    board[0][3] = Piece::BQUEEN;
    board[0][4] = Piece::BKING;
    for (int c = 0; c < 8; c++)
    {
        board[1][c] = Piece::BPAWN;
    }

    // 白方棋子
    board[7][0] = board[7][7] = Piece::WROOK;
    board[7][1] = board[7][6] = Piece::WKNIGHT;
    board[7][2] = board[7][5] = Piece::WBISHOP;
    board[7][3] = Piece::WQUEEN;
    board[7][4] = Piece::WKING;
    for (int c = 0; c < 8; c++)
    {
        board[6][c] = Piece::WPAWN;
    }

    for (int i = 1; i <= 12; i++)
    {
        pieceCount[i] = 0;
        for (int j = 0; j < 10; j++)
        {
            piecePos[i][j] = {-1, -1};
        }
    }
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board[r][c];
            if (p == Piece::EMPTY)
                continue;
            piecePos[pieceToIndex(p)][pieceCount[pieceToIndex(p)]++] = {r, c};
        }
    }

    materialScore = 0;
    PSTScore = 0;
    // 所有 castle 都可能
    castleRights = 0b1111;

    initZobrist();
    zobristKey = computeZobrist(*this);

    enPassantPos = POS_NONE;

    clearRepetitionKey();
    pushRepetitionKey();
}

// 回傳位於 pos 的 Piece
Piece Board::at(Position pos) const
{
    if (pos.row < 0 || pos.row >= 8)
    {
        ENGINE_FATAL("board", "invalid row: ", pos);
    }
    if (pos.col < 0 || pos.col >= 8)
    {
        ENGINE_FATAL("board", "invalid col: ", pos);
    }

    return board[pos.row][pos.col];
}

// 設定 Piece 在 pos 裡
void Board::set(Position pos, Piece p)
{
    board[pos.row][pos.col] = p;
}

void computePiecePos(Board& board)
{
    for (int i = 1; i <= 12; i++)
    {
        board.pieceCount[i] = 0;
        for (int j = 0; j < 10; j++)
        {
            board.piecePos[i][j] = {-1, -1};
        }
    }
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board.board[r][c];
            if (p == Piece::EMPTY)
                continue;

            int pIndex = pieceToIndex(p);
            board.piecePos[pIndex][board.pieceCount[pIndex]++] = {r, c};
            // std::cout << p << ' ' << r << ' ' << c << '\n';
        }
    }
}