#include "search/Zobrist.h"
#include "board/Board.h"
#include <random>

uint64_t zobPiece[PIECE_NB][SQUARE_NB];
uint64_t zobPlayer;
uint64_t zobCastle[16];
uint64_t zobEnPassant[8];

uint64_t rand64()
{
    static std::mt19937_64 rng(114514);
    static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    return dist(rng);
}

void initZobrist()
{
    for (int p = 1; p < PIECE_NB; p++)
        for (int s = 0; s < 64; s++)
            zobPiece[p][s] = rand64();

    zobPlayer = rand64();

    for (int i = 0; i < 16; i++)
        zobCastle[i] = rand64();
}

uint64_t computeZobrist(const Board& board)
{
    uint64_t key = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece piece = board.at({r, c});
            if (piece != Piece::EMPTY)
            {
                key ^= zobPiece[pieceToIndex(piece)][zobBoardPosition({r, c})];
            }
        }
    }

    if (board.player == Player::BLACK)
        key ^= zobPlayer;

    key ^= zobCastle[board.castleRights];

    return key;
}

int zobBoardPosition(Position pos)
{
    return pos.row * 8 + pos.col;
}