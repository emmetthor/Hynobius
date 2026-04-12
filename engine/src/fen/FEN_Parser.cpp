#include "fen/FEN_Parser.h"
#include "Structure_IO.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"
#include <iostream>
#include <sstream>

Board cinFenToBoard(const std::string& fen)
{
    Board board;

    std::istringstream iss(fen);
    std::string boardStr, player, castling, enpass, halfmove, fullmove;
    if (!(iss >> boardStr >> player >> castling >> enpass >> halfmove >> fullmove))
    {
        ENGINE_FATAL("fen parser", "FEN is incomplete.");
    }

    int row = 0, col = 0;
    for (auto c : boardStr)
    {
        if (c == '/')
        {
            row++;
            col = 0;
        }

        else if (isdigit(c))
        {
            for (int i = 0; i < c - '0'; i++)
            {
                board.board[row][col + i] = Piece::EMPTY;
            }
            col += c - '0';
        }

        else
        {
            board.board[row][col++] = makePiece((isupper(c) ? Player::WHITE : Player::BLACK), c);
        }
    }
    ENGINE_ASSERT(row == 7 && col == 8);

    DOUT("fen parser") << "current board:\n" << board << '\n';

    ENGINE_ASSERT(player == "w" || player == "b");

    board.player = (player == "w" ? Player::WHITE : Player::BLACK);

    board.castleRights = 0;
    for (char c : castling)
    {
        switch (c)
        {
            case 'K':
                board.castleRights |= 0b0100;
                break;
            case 'Q':
                board.castleRights |= 0b1000;
                break;
            case 'k':
                board.castleRights |= 0b0001;
                break;
            case 'q':
                board.castleRights |= 0b0010;
                break;
            case '-':
                break;
            default:
                ENGINE_FATAL("fen parser", "FEN castling is incomplete.", c);
                break;
        }
    }

    if (enpass != "-")
    {
        board.enPassantPos = {8 - (enpass[1] - '0'), (enpass[0] - 'a')};
    }
    else
    {
        board.enPassantPos = POS_NONE;
    }

    // initialize variables.
    board.materialScore = computePieceValue(board);
    board.PSTScore = computePST(board);
    board.zobristKey = computeZobrist(board);
    computePiecePos(board);

    board.pushRepetitionKey();

    return board;
}