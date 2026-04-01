#pragma once
#include "board/Board.h"
#include <cstdint>
#include <board/Piece.h>

struct UndoState {
    uint8_t castleRights;
    int materialScore;
    int PSTScore;
    uint64_t zobristKey;
    Piece capturedPiece;
};

// intergret move infos
// placedPiece: The piece that actually placed at Position to
struct MoveState {
    Position from, to;

    Piece movePiece, capturedPiece, placedPiece;

    Player player;
    
    bool isCastle, isPromotion;
};