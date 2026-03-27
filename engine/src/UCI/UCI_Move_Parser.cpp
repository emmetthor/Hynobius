#include "UCI/UCI_Move_Parcer.h"

Move parseCastle(Move& move, const Board& board)
{
    Piece p = move.movePiece;

    if (p == Piece::WKING || p == Piece::BKING)
    {

        // 白短易位
        if (move.from.row == 0 && move.from.col == 4 && move.to.row == 0 && move.to.col == 6)
        {
            move.castle = Castle::SHORT_CASTLE;
        }

        // 白長易位
        if (move.from.row == 0 && move.from.col == 4 && move.to.row == 0 && move.to.col == 2)
        {
            move.castle = Castle::LONG_CASTLE;
        }

        // 黑短易位
        if (move.from.row == 7 && move.from.col == 4 && move.to.row == 7 && move.to.col == 6)
        {
            move.castle = Castle::SHORT_CASTLE;
        }

        // 黑長易位
        if (move.from.row == 7 && move.from.col == 4 && move.to.row == 7 && move.to.col == 2)
        {
            move.castle = Castle::LONG_CASTLE;
        }
    }

    return move;
}

Move parseUCIMove(const std::string strMove, const Board& board)
{
    Move move;
    move.from = {8 - (strMove[1] - '0'), strMove[0] - 'a'};
    move.to = {8 - (strMove[3] - '0'), strMove[2] - 'a'};
    move.player = board.player;
    move.movePiece = board.at(move.from);
    move.capturePiece = board.board[move.to.row][move.to.col];

    move = parseCastle(move, board);

    if (strMove.length() == 5)
    {
        move.isPromotion = true;

        char promoChar = strMove[4];
        Player player = board.player;

        switch (promoChar)
        {
            case 'q':
                move.promotionPiece = makePiece(player, 'Q');
                break;
            case 'r':
                move.promotionPiece = makePiece(player, 'R');
                break;
            case 'b':
                move.promotionPiece = makePiece(player, 'B');
                break;
            case 'n':
                move.promotionPiece = makePiece(player, 'N');
                break;
        }
    }

    return move;
}