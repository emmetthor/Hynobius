// INFO this file is not used in the engine, but the code is useful.
// #include "pgn/Pgn_Parser.h"
// #include "board/Board.h"
// #include "move/Make_Move.h"
// #include "pgn/Pgn_Transformer.h"

// #include <iostream>
// #include <map>
// #include <string>
// #include <vector>

// enum Header
// {
//     NONE,
//     USELESS,
//     WHITE,
//     BLACK,
//     WHITEELO,
//     BLACKELO,
// };

// std::map<std::string, Castle> castleTypeMap = {{"O-O", SHORT_CASTLE}, {"O-O-O", LONG_CASTLE}};

// std::map<std::string, Header> headerTypeMap = {
//     {"[White", WHITE}, {"[Black", BLACK}, {"[WhiteElo", WHITEELO}, {"[BlackElo", BLACKELO}};

// std::map<char, bool> specialPiece = {
//     {'K', 1},
//     {'Q', 1},
//     {'N', 1},
//     {'B', 1},
//     {'R', 1},
// };

// std::string sanClearer(std::string strSan)
// {
//     int sanSize = strSan.size();
//     std::string res = "";
//     for (int i = 0; i < sanSize; i++)
//     {
//         if (isdigit(strSan[i]) || isalpha(strSan[i]) || strSan[i] == '=' || strSan[i] == '-')
//         {
//             res += strSan[i];
//         }
//     }

//     return res;
// }

// void PGN::cinPgnToSan()
// {
//     std::string input;
//     int headerType = NONE;

//     Board board;

//     while (std::cin >> input)
//     {
//         Move move;

//         // 查看 header [xxx 是否符合
//         if (headerTypeMap.find(input) != headerTypeMap.end())
//         {
//             headerType = headerTypeMap[input];
//             continue;
//         }

//         if (input[0] == '[')
//         {
//             headerType = USELESS;
//             // debug::log("cinPgnToSan: useless header\n");
//             continue;
//         }

//         // 接續 header
//         if (headerType != NONE)
//         {
//             int sz = input.size();

//             switch (headerType)
//             {
//                 case WHITE:
//                     whiteName = input.substr(1, sz - 3);
//                     break;
//                 case BLACK:
//                     blackName = input.substr(1, sz - 3);
//                     break;
//                 case WHITEELO:
//                     whiteElo = input.substr(1, sz - 3);
//                     break;
//                 case BLACKELO:
//                     blackElo = input.substr(1, sz - 3);
//                     break;

//                 default:
//                     break;
//             }

//             if (input.back() == ']')
//             {
//                 headerType = NONE;
//             }

//             continue;
//         }

//         // 1-0 or 0-1
//         if (input.size() == 3 && isdigit(input[0]) && isdigit(input[2]))
//         {
//             break;
//         }

//         // 1. 2. 3. ...
//         if (isdigit(input[0]))
//         {
//             continue;
//         }

//         san_moves.push_back(sanClearer(input));
//     }

//     // debug::log("cinPgnToSan: ", whiteName, ' ', whiteElo, ' ', blackName, ' ', blackElo,
//     '\n');
// }

// bool isCapture(std::string strSan)
// {
//     bool res = 0;

//     for (int i = 0; i < (int)strSan.size(); i++)
//     {
//         if (strSan[i] == 'x')
//             res = 1;
//     }

//     return res;
// }

// bool isPromote(std::string strSan)
// {
//     int sanSize = strSan.size();

//     for (int i = 0; i < sanSize; i++)
//     {
//         if (strSan[i] == '=')
//             return true;
//     }

//     return false;
// }

// SAN parsePieceSan(std::string strSan, Player player)
// {
//     SAN san;

//     san.piece = makePiece(player, strSan[0]);

//     int sanSize = strSan.size();

//     // skip strSan[0]
//     int i = 1;

//     if (sanSize > 3 && strSan[i] != 'x' && isalpha(strSan[i]))
//     {
//         san.fromPos.col = strSan[i] - 'a';
//         i++;
//     }

//     if (strSan[i] != 'x' && isdigit(strSan[i]))
//     {
//         san.fromPos.row = 8 - (strSan[i] - '0');
//         i++;
//     }

//     if (strSan[i] == 'x')
//     {
//         san.isCapture = true;
//         i++;
//     }

//     san.toPos = pgnToPosition(strSan.substr(i, 2));

//     return san;
// }

// Move parsePieceMove(SAN& strSan, Board& board)
// {
//     for (int r = 0; r < 8; r++)
//     {
//         for (int c = 0; c < 8; c++)
//         {
//             if (board.at({r, c}) != strSan.piece)
//                 continue;
//             if (strSan.fromPos.row != -1 && r != strSan.fromPos.row)
//                 continue;
//             if (strSan.fromPos.col != -1 && c != strSan.fromPos.col)
//                 continue;

//             Move move;
//             move.from = {r, c};
//             move.movePiece = strSan.piece;
//             move.player = isWhite(strSan.piece) ? Player::WHITE : Player::BLACK;
//             move.to = strSan.toPos;
//             move.capturePiece = board.at(move.to);

//             if (isMoveLegal(board, move))
//             {
//                 return move;
//             }
//         }
//     }

//     // debug::log("parsePieceMove: No valid move\n");

//     return Move();
// }

// SAN parsePawnSan(std::string strSan, Player player)
// {
//     SAN san;

//     san.piece = makePiece(player, 'P');

//     int sanSize = strSan.size();

//     // skip strSan[0]
//     int i = 0;

//     if (sanSize > 3 && strSan[i] != 'x' && isalpha(strSan[i]))
//     {
//         san.fromPos.col = strSan[i] - 'a';
//         i++;
//     }

//     if (strSan[i] != 'x' && isdigit(strSan[i]))
//     {
//         san.fromPos.col = 8 - (strSan[i] - '0');
//         i++;
//     }

//     if (strSan[i] == 'x')
//     {
//         san.isCapture = true;
//         i++;
//     }

//     san.toPos = pgnToPosition(strSan.substr(i, 2));

//     i += 2;

//     if (sanSize >= i && strSan[i] == '=')
//     {
//         san.isPromote = true;
//         i++;
//     }

//     if (san.isPromote == true)
//     {
//         san.promorePiece = makePiece(player, strSan[i]);
//     }

//     return san;
// }

// Move parsePawnMove(SAN& san, Board& board)
// {
//     for (int r = 0; r < 8; r++)
//     {
//         for (int c = 0; c < 8; c++)
//         {
//             if (board.at({r, c}) != san.piece)
//                 continue;
//             if (san.fromPos.row != -1 && r != san.fromPos.row)
//                 continue;
//             if (san.fromPos.col != -1 && c != san.fromPos.col)
//                 continue;

//             Move move;
//             move.from = {r, c};
//             move.movePiece = san.piece;
//             move.player = isWhite(san.piece) ? Player::WHITE : Player::BLACK;
//             move.to = san.toPos;
//             move.capturePiece = board.at(move.to);
//             move.isPromotion = san.isPromote;
//             move.promotionPiece = san.promorePiece;

//             if (isMoveLegal(board, move))
//             {
//                 return move;
//             }
//         }
//     }

//     // debug::log("parsePawnMove: No valid move\n");

//     return Move();
// }

// Move parseCastleSan(std::string san, Player player)
// {
//     Move move;
//     move.player = player;

//     if (san == "O-O")
//     {
//         move.castle = SHORT_CASTLE;
//     }

//     else if (san == "O-O-O")
//     {
//         move.castle = LONG_CASTLE;
//     }

//     return move;
// }

// void PGN::SantoMove()
// {
//     Player player = Player::WHITE;

//     Board board;

//     for (auto san : san_moves)
//     {
//         // debug::log("SantoMove: ", san, '\n');

//         if (specialPiece.find(san[0]) != specialPiece.end())
//         {
//             SAN pieceSan = parsePieceSan(san, player);
//             Move pieceMove = parsePieceMove(pieceSan, board);

//             makeMove(board, pieceMove);

//             moves.emplace_back(pieceMove);
//         }

//         else if (san[0] == 'O')
//         {
//             // castle
//             Move castleMove = parseCastleSan(san, player);

//             makeMove(board, castleMove);

//             moves.emplace_back(castleMove);
//         }

//         else
//         {
//             // pawn
//             SAN pawnSan = parsePawnSan(san, player);
//             Move pawnMove = parsePawnMove(pawnSan, board);

//             makeMove(board, pawnMove);

//             moves.emplace_back(pawnMove);
//         }

//         player = (player == Player::WHITE ? Player::BLACK : Player::WHITE);
//     }
// }

// Move PGN::SantoMoveSingle(Board& board, std::string strSan, Player player)
// {
//     // debug::log("SantoMove: ", strSan, '\n');

//     if (specialPiece.find(strSan[0]) != specialPiece.end())
//     {
//         SAN pieceSan = parsePieceSan(strSan, player);
//         Move pieceMove = parsePieceMove(pieceSan, board);

//         makeMove(board, pieceMove);

//         return pieceMove;
//     }

//     else if (strSan[0] == 'O')
//     {
//         // castle
//         Move castleMove = parseCastleSan(strSan, player);

//         makeMove(board, castleMove);

//         return castleMove;
//     }

//     else
//     {
//         // pawn
//         SAN pawnSan = parsePawnSan(strSan, player);
//         Move pawnMove = parsePawnMove(pawnSan, board);

//         makeMove(board, pawnMove);

//         return pawnMove;
//     }
// }

// std::vector<Move> PGN::getMoves()
// {
//     return moves;
// }

// int PGN::getMovesCount()
// {
//     return moves.size();
// }
