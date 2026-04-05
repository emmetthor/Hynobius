#include "UCI/UCI.h"
#include "Structure_IO.h"
#include "UCI/UCI_Move_Parcer.h"
#include "move/Move.h"
#include "pgn/Pgn_Transformer.h"

#include <iostream>
#include <sstream>
#include <string>

std::string castleMap[2][3]{{"invalid", "e1g1", "e1c1"}, {"invalid", "e8g8", "e8c8"}};

std::string UCIMoveToString(const Move& move)
{
    std::string res;
    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        res += castleMap[playerToIndex(move.player)][static_cast<int>(move.castle)];
    }
    else
    {
        res += positionToPgn(move.from) + positionToPgn(move.to);
    }

    if (move.isPromotion)
    {
        char promotionChar = '.';
        switch (pieceToChar(move.promotionPiece))
        {
            case 'Q':
            case 'q':
                promotionChar = 'q';
                break;
            case 'R':
            case 'r':
                promotionChar = 'r';
                break;
            case 'N':
            case 'n':
                promotionChar = 'n';
                break;
            case 'B':
            case 'b':
                promotionChar = 'b';
                break;
            default:
                ENGINE_FATAL(
                    "uci", "promotion piece is not valid: ", move.promotionPiece);
        }
        res += promotionChar;
    }

    return res;
}

void handleGo(std::istringstream& iss, Engine& engine)
{
    int depth = 6;
    std::string token;

    while (iss >> token)
    {
        if (token == "depth")
        {
            iss >> depth;
        }
    }

    Move move = engine.goDepth(depth);

    std::cout << "bestmove " << UCIMoveToString(move) << '\n';
}

void handlePosition(std::istringstream& iss, Engine& engine)
{
    std::string token, fen;
    iss >> token;

    if (token == "startpos")
    {
        engine.setStartPosition();
    }
    else if (token == "fen")
    {
        for (int t = 0; t < 6; t++)
        {
            iss >> token;
            if (!fen.empty())
                fen += " ";
            fen += token;
        }
        engine.setPositionWithFen(fen);
    }
    else
    {
        ENGINE_FATAL("uci", "Invalid UCI position command.");
    }

    // no moves command
    if (!(iss >> token))
    {
        return;
    }

    if (token == "moves")
    {
        std::string strMove;
        while (iss >> strMove)
        {
            Move move = parseUCIMove(strMove, engine.getBoard());
            engine.move(move);
        }
    }
}

void handleBench(std::istringstream& iss, Engine& engine)
{
    std::string token;
    iss >> token;
    // TODO
}

void uciLoop(Engine& engine)
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci")
        {
            std::cout << "id name engine1\nid author EmmetThor\n";
            std::cout << "uciok\n" << std::flush;
        }
        else if (token == "isready")
        {
            std::cout << "readyok\n" << std::flush;
        }
        else if (token == "position")
        {
            handlePosition(iss, engine);
        }
        else if (token == "go")
        {
            handleGo(iss, engine);
        }
        else if (token == "quit" || token == "stop")
        {
            break;
        }
        else if (token == "PRINTBOARD")
        {
            std::cout << engine.getBoard() << '\n';
        }
        else if (token == "bench")
        {
            handleBench(iss, engine);
        }
        else
        {
            std::cerr << "Unrecognized token.\n" << std::flush;
        }
    }
}