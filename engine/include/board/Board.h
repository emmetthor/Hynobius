#pragma once

#include "Type.h"
#include "Piece.h"

#include <string>
#include <cstdint>

/*
初始化
輸出棋盤

取的 (r, c) 棋子
放置棋子 p 於 (r, c)

是否白方先手
*/

struct Position {
    int row = -1;
    int col = -1;

    bool operator==(const Position &other) const {
        return
            row == other.row &&
            col == other.col;
    }
};

inline bool isInBoard(Position pos) {
    return 0 <= pos.row && pos.row < 8 && 0 <= pos.col && pos.col < 8;
}

inline bool isPlayerValid(Player player) {
    return player == Player::WHITE || player == Player::BLACK;
}

inline Player opponent(Player player) {
    return (player == Player::WHITE ? Player::BLACK : Player::WHITE);
}

inline Piece makePiece(Player player, char pieceChar) {
    return MAKE_PIECE_MAP[static_cast<int>(player)][charToPieceIndex(pieceChar)];
}

inline int playerToIndex(Player player) {
    return static_cast<int>(player);
}

struct Board {
    Board();

    void init();

    Piece at(Position pos) const;
    void set(Position pos, Piece p);
    
    Piece board[8][8];
    Position piecePos[13][10];
    int pieceCount[13] = {};
    int materialScore;
    int PSTScore;
    Player player;
    
    /*
    0001 黑方 queen side
    0010 黑方 king  side
    0100 白方 queen side
    1000 白方 king  side
    */
    int castleRights;
    uint64_t zobristKey;

    inline void piecePosDelete(Position *posArray, int &count, const Position &target) {
        for (int i = 0; i < count; i++) {
            if (posArray[i] == target) {
                posArray[i] = posArray[count - 1];
                count--;
                return;
            }
        }

        ENGINE_FATAL(DebugCategory::BOARD, "can't find piece position");
    }
    inline void piecePosAdd(Position *posArray, int &count, const Position &add) {
        posArray[count++] = add;
    }

    inline const Position* getPiecePos(Piece p) const {
        return piecePos[pieceToIndex(p)];
    }
    inline const int getPieceCount(Piece p) const {
        int pIndex = pieceToIndex(p);
        if (!(1 <= pIndex && pIndex <= 12)) {
            ENGINE_FATAL(DebugCategory::BOARD, "pIndex is empty or invalid: ", pIndex);
        }
        return pieceCount[pIndex];
    }
};

bool validatePiecePos(const Board &b);

void computePiecePos(Board &board);