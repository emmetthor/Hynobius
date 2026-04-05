#pragma GCC optimize("O3,unroll-loops")

#include "board/Attack.h"
#include "board/Board.h"
#include "debug.h"
#include "move/Move_Direction.h"

int countPawnAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    int pdr = (player == Player::WHITE ? -1 : 1);
    for (int pdc : {-1, 1})
    {
        Position p = {pos.row - pdr, pos.col + pdc};
        if (!isInBoard(p))
            continue;

        if (board.at(p) == makePiece(player, 'P'))
            cnt++;
    }

    return cnt;
}

int countKingAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    Piece king = makePiece(player, 'K');

    for (int i = 0; i < 8; i++)
    {
        Position p = {pos.row + MoveDirection::KING_DR[i], pos.col + MoveDirection::KING_DC[i]};

        if (isInBoard(p) && board.at(p) == king)
            cnt++;
    }

    return cnt;
}

int countKnightAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    Piece knight = makePiece(player, 'N');

    for (int i = 0; i < 8; i++)
    {
        Position p = {pos.row + MoveDirection::KNIGHT_DR[i], pos.col + MoveDirection::KNIGHT_DC[i]};

        if (isInBoard(p) && board.at(p) == knight)
            cnt++;
    }

    return cnt;
}

int countDiagnalAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    Piece bishop = makePiece(player, 'B');
    Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        Position p = {pos.row + MoveDirection::BISHOP_QUEEN_DR[i],
                      pos.col + MoveDirection::BISHOP_QUEEN_DC[i]};

        while (true)
        {
            if (!isInBoard(p))
                break;

            Piece pp = board.at(p);

            if (pp == bishop || pp == queen)
                cnt++;

            if (pp != Piece::EMPTY)
                break;

            p.row += MoveDirection::BISHOP_QUEEN_DR[i];
            p.col += MoveDirection::BISHOP_QUEEN_DC[i];
        }
    }

    return cnt;
}

int countStraightAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    Piece rook = makePiece(player, 'R');
    Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        Position p = {pos.row + MoveDirection::ROOK_QUEEN_DR[i],
                      pos.col + MoveDirection::ROOK_QUEEN_DC[i]};

        while (true)
        {
            if (!isInBoard(p))
                break;

            Piece pp = board.at(p);

            if (pp == rook || pp == queen)
                cnt++;

            if (pp != Piece::EMPTY)
                break;

            p.row += MoveDirection::ROOK_QUEEN_DR[i];
            p.col += MoveDirection::ROOK_QUEEN_DC[i];
        }
    }

    return cnt;
}

// int PawnAttackers(
//     const Board &board,
//     Position pos,
//     const Player player,
//     Position *buffer
// ) {
//     // 假設 pos 與 player 合法，且 buffer 空間足夠
//     // 枚舉所有兵的合法走法 O(2)
//     int cnt = 0;
//     int pdr = (player == Player::WHITE ? -1 : 1);
//     for (int pdc : {-1, 1}) {
//         Position p = {pos.row - pdr, pos.col + pdc};
//         if (!isInBoard(p)) continue;

//         if (board.at(p) == makePiece(player, 'P')) buffer[cnt++] = p;
//     }

//     return cnt;
// }

// int KnightAttackers(
//     const Board &board,
//     Position pos,
//     const Player player,
//     Position *buffer
// ) {
//     // 假設 pos 與 player 合法，且 buffer 空間足夠
//     // 枚舉所有騎士的合法走法 O(8)
//     int cnt = 0;
//     Piece knight = makePiece(player, 'N');

//     for (int i = 0; i < 8; i++) {
//         Position p = {pos.row + MoveDirection::KNIGHT_DR[i], pos.col +
//         MoveDirection::KNIGHT_DC[i]};

//         if (isInBoard(p) && board.at(p) == knight) buffer[cnt++] = p;
//     }

//     return cnt;
// }

// int BishopAttackers(
//     const Board &board,
//     Position pos,
//     const Player player,
//     Position *buffer
// ) {
//     // 假設 pos 與 player 合法，且 buffer 空間足夠
//     // 枚舉所有主教的合法走法 O(16)
//     int cnt = 0;
//     Piece bishop = makePiece(player, 'B');

//     for (int i = 0; i < 4; i++) {
//         Position p = {pos.row + MoveDirection::BISHOP_QUEEN_DR[i], pos.col +
//         MoveDirection::BISHOP_QUEEN_DC[i]};

//         while (true) {
//             if (!isInBoard(p)) break;

//             Piece pp = board.at(p);

//             if (pp == bishop) buffer[cnt++] = p;

//             if (pp != Piece::EMPTY) break;

//             p.row += MoveDirection::BISHOP_QUEEN_DR[i];
//             p.col += MoveDirection::BISHOP_QUEEN_DC[i];
//         }
//     }

//     return cnt;
// }

// int RookAttackers(
//     const Board &board,
//     Position pos,
//     const Player player,
//     Position *buffer
// ) {
//     // 假設 pos 與 player 合法，且 buffer 空間足夠
//     // 枚舉所有城堡的合法走法 O(16)
//     int cnt = 0;
//     Piece rook = makePiece(player, 'R');

//     for (int i = 0; i < 4; i++) {
//         Position p = {pos.row + MoveDirection::ROOK_QUEEN_DR[i], pos.col +
//         MoveDirection::ROOK_QUEEN_DC[i]};

//         while (true) {
//             if (!isInBoard(p)) break;

//             Piece pp = board.at(p);

//             if (pp == rook) buffer[cnt++] = p;

//             if (pp != Piece::EMPTY) break;

//             p.row += MoveDirection::ROOK_QUEEN_DR[i];
//             p.col += MoveDirection::ROOK_QUEEN_DC[i];
//         }
//     }

//     return cnt;
// }

// int QueenAttackers(
//     const Board &board,
//     Position pos,
//     const Player player,
//     Position *buffer
// ) {
//     // 假設 pos 與 player 合法，且 buffer 空間足夠
//     // 枚舉所有皇后的合法走法 O(32)
//     int cnt = 0;
//     Piece queen = makePiece(player, 'Q');

//     for (int i = 0; i < 4; i++) {
//         Position p = {pos.row + MoveDirection::BISHOP_QUEEN_DR[i], pos.col +
//         MoveDirection::BISHOP_QUEEN_DC[i]};

//         while (true) {
//             if (!isInBoard(p)) break;

//             Piece pp = board.at(p);

//             if (pp == queen) buffer[cnt++] = p;

//             if (pp != Piece::EMPTY) break;

//             p.row += MoveDirection::BISHOP_QUEEN_DR[i];
//             p.col += MoveDirection::BISHOP_QUEEN_DC[i];
//         }
//     }

//     for (int i = 0; i < 4; i++) {
//         Position p = {pos.row + MoveDirection::ROOK_QUEEN_DR[i], pos.col +
//         MoveDirection::ROOK_QUEEN_DC[i]};

//         while (true) {
//             if (!isInBoard(p)) break;

//             Piece pp = board.at(p);

//             if (pp == queen) buffer[cnt++] = p;

//             if (pp != Piece::EMPTY) break;

//             p.row += MoveDirection::ROOK_QUEEN_DR[i];
//             p.col += MoveDirection::ROOK_QUEEN_DC[i];
//         }
//     }

//     return cnt;
// }

// Calculate attacks in the designated square.
int countSquareAttacks(const Board& board, Position pos, const Player player)
{
    // check position.
    if (!isInBoard(pos))
    {
        ENGINE_FATAL("attack", "position is not in board.");
    }

    int cnt = 0;
    cnt += countPawnAttacks(board, pos, player);
    cnt += countKnightAttacks(board, pos, player);
    cnt += countDiagnalAttacks(board, pos, player);
    cnt += countStraightAttacks(board, pos, player);
    cnt += countKingAttacks(board, pos, player);
    return cnt;
}

// 給 buffer 填充此格子的攻擊者位置
// WARN 未寫 King 的 get attackers
// int getAttackers(
//     const Board &board,
//     Position pos,
//     Player player,
//     Position* buffer
// ) {
//     // 檢查位置合法性
//     if (!isInBoard(pos)) {
//         LOG_ERROR(DebugCategory::ATK, "position is invalid", pos);
//         ENGINE_ASSERT(isInBoard(pos));
//     }
//     // 檢查玩家合法性
//     if (!isPlayerValid(player)) {
//         LOG_ERROR(DebugCategory::ATK, "player is invalid", player);
//         ENGINE_ASSERT(isPlayerValid(player));
//     }

//     int cnt = 0;
//     cnt += PawnAttackers(board, pos, player, buffer + cnt);
//     cnt += KnightAttackers(board, pos, player, buffer + cnt);
//     cnt += BishopAttackers(board, pos, player, buffer + cnt);
//     cnt += RookAttackers(board, pos, player, buffer + cnt);
//     return cnt;
// }

// Check whether the designated square is attacked.
bool isSquareAttacked(const Board& board, Position pos, const Player player)
{
    // check position
    if (!isInBoard(pos))
    {
        ENGINE_FATAL("attack", "position is not in board.");
    }

    return (countSquareAttacks(board, pos, player) > 0 ? 1 : 0);
}
