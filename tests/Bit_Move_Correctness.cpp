#include "../engine/include/board/Piece.h"
#include "../engine/include/move/Move.h"

int main()
{
    for (Square from = 0; from < 64; from++)
    {
        for (Square to = 0; to < 64; to++)
        {
            for (int pieceIndex = 1; pieceIndex <= 12; pieceIndex++)
            {
                for (int flag = 0; flag < (1 << 4); flag++)
                {
                    bool capture = flag & 1;
                    bool castle = flag & 2;
                    bool enPassant = flag & 4;
                    bool promote = flag & 8;

                    BitMove m = makeBitMove(from,
                                            to,
                                            static_cast<Piece>(pieceIndex),
                                            capture,
                                            castle,
                                            enPassant,
                                            promote);

                    ENGINE_ASSERT(getFromSquare(m) == from);
                    ENGINE_ASSERT(getToSquare(m) == to);
                    ENGINE_ASSERT(getPromotePiece(m) == static_cast<Piece>(pieceIndex));
                    ENGINE_ASSERT(getCapture(m) == capture);
                    ENGINE_ASSERT(getCastle(m) == castle);
                    ENGINE_ASSERT(getEnPassant(m) == enPassant);
                    ENGINE_ASSERT(getPromotion(m) == promote);
                }
            }
        }
    }
}