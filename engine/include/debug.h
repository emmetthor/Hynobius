// #pragma once

// #include <fstream>
// #include <sstream>
// #include <iostream>
// #include <iomanip>
// #include <cstdint>
// #include <utility>  // std::forward

// #include "board/Piece.h"
// #include "board/Board.h"
// #include "move/Move.h"
// #include "pgn/Pgn_Transformer.h"

// // ======================
// // DEBUG 開關
// // ======================
// #ifdef ENGINE_DEBUG
//     #define DEBUG_ENABLED 1
// #else
//     #define DEBUG_ENABLED 0
// #endif

// // ======================
// // assert
// // ======================
// #ifdef ENGINE_DEBUG
//     #include <assert.h>
//     #define ENGINE_ASSERT(x) assert(x)
// #else
//     #define ENGINE_ASSERT(x) ((void)0)
// #endif

// // ======================
// // Debug 等級與分類
// // ======================
// enum class DebugLevel { INFO, DEBUG, WARN, ERROR };
// enum class DebugCategory { TT, MOVE, QS, EVAL, BOARD, SEARCH, ATK };

// // ======================
// // operator<< for structures
// // ======================
// #if DEBUG_ENABLED
// inline std::ostream& operator<<(std::ostream& os, const Position& p) {
//     return os << "(" << p.row << "," << p.col << ")";
// }
// #endif

// #if DEBUG_ENABLED
// inline std::ostream& operator<<(std::ostream& os, const Board& p) {
//     for (int r = 0; r < 9; r++) {
//         for (int c = -1; c < 8; c++) {
//             if (r == 8 && c == -1) {
//                 os << "  ";
//                 continue;
//             }

//             if (c == -1) {
//                 os << rowToPgn[r] << ' ';
//                 continue;
//             }

//             if (r == 8) {
//                 os << colToPgn[c] << ' ';
//                 continue;
//             }

//             os << pieceToChar(p.board[r][c]) << " \n"[c == 8 - 1];
//         }
//     }

//     return os;
// }
// #endif

// #if DEBUG_ENABLED
// inline std::ostream& operator<<(std::ostream& os, const Move& move) {
//     if (move.castle == SHORT_CASTLE) os << "O-O";
//     else if (move.castle == LONG_CASTLE) os << "O-O-O";
//     else os << pieceToChar(move.movePiece) << positionToPgn(move.from) << positionToPgn(move.to);
//     return os;
// }
// #endif

// // ======================
// // Debug class
// // ======================
// class Debug {
// public:
//     static DebugLevel level;
//     static uint32_t categoryMask;
//     static bool consoleEnabled;
//     static bool fileEnabled;

//     static void init(const char* file = nullptr) {
// #if DEBUG_ENABLED
//         if (file) {
//             g_logFile.open(file);
//             fileEnabled = g_logFile.is_open();
//         }
// #endif
//     }

//     static void shutdown() {
// #if DEBUG_ENABLED
//         if (g_logFile.is_open()) g_logFile.close();
// #endif
//     }

// #if DEBUG_ENABLED
//     static void logHeader(std::ostringstream& oss,
//                           DebugCategory cat,
//                           DebugLevel lvl,
//                           const char* file,
//                           int line,
//                           const char* func) {
//         oss << "[" << std::left << std::setw(7) << categoryToStr(cat)
//             << "][" << std::setw(5) << levelToStr(lvl)
//             << "][" << std::setw(20) << (std::string(file) + ":" + std::to_string(line))
//             << "][" << std::setw(12) << func << "] ";
//     }

//     template <typename... Args>
//     static void logStream(DebugCategory cat,
//                           DebugLevel lvl,
//                           const char* file,
//                           int line,
//                           const char* func,
//                           Args&&... args) {
//         if (lvl < level) return;
//         if (!(categoryMask & (1u << int(cat)))) return;

//         std::ostringstream oss;
//         logHeader(oss, cat, lvl, file, line, func);
//         (oss << ... << std::forward<Args>(args)); // fold expression

//         if (consoleEnabled) std::cout << oss.str() << '\n';
//         if (fileEnabled && g_logFile.is_open()) g_logFile << oss.str() << '\n';
//     }

// private:
//     static const char* levelToStr(DebugLevel lvl) {
//         switch (lvl) {
//             case DebugLevel::INFO:  return "INFO";
//             case DebugLevel::DEBUG: return "DEBUG";
//             case DebugLevel::WARN:  return "WARN";
//             case DebugLevel::ERROR: return "ERROR";
//         }
//         return "UNK";
//     }

//     static const char* categoryToStr(DebugCategory cat) {
//         switch (cat) {
//             case DebugCategory::TT:     return "TT";
//             case DebugCategory::MOVE:   return "MOVE";
//             case DebugCategory::QS:     return "QS";
//             case DebugCategory::EVAL:   return "EVAL";
//             case DebugCategory::BOARD:  return "BOARD";
//             case DebugCategory::SEARCH: return "SEARCH";
//             case DebugCategory::ATK:    return "ATK";
//         }
//         return "UNK";
//     }

//     static std::ofstream g_logFile;
// #endif
// };

// // ======================
// // static members
// // ======================
// #if DEBUG_ENABLED
// inline std::ofstream Debug::g_logFile;
// #endif

// inline DebugLevel Debug::level = DebugLevel::DEBUG;
// inline uint32_t Debug::categoryMask = 0xFFFFFFFF;
// inline bool Debug::consoleEnabled = true;
// inline bool Debug::fileEnabled = false;

// // ======================
// // macro
// // ======================
// #if DEBUG_ENABLED
// #define LOG_DEBUG(cat, ...) \
//     Debug::logStream(cat, DebugLevel::DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

// #define LOG_INFO(cat, ...) \
//     Debug::logStream(cat, DebugLevel::INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)

// #define LOG_WARN(cat, ...) \
//     Debug::logStream(cat, DebugLevel::WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)

// #define LOG_ERROR(cat, ...) \
//     Debug::logStream(cat, DebugLevel::ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
// #else
// #define LOG_DEBUG(...)
// #define LOG_INFO(...)
// #define LOG_WARN(...)
// #define LOG_ERROR(...)
// #endif

#pragma once
#include <cassert>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>

#ifdef ENGINE_DEBUG
    #define ENGINE_ASSERT(x) assert(x)
#else
    #define ENGINE_ASSERT(x) ((void)0)
#endif

enum class DebugLevel { INFO, DEBUG, WARN, ERROR };
enum class DebugCategory { TT, MOVE, QS, EVAL, BOARD, SEARCH, ATK, GENERATE, PIECE};

class Debug {
public:
    static DebugLevel level;
    static uint32_t categoryMask;

    template <typename... Args>
    static void logStream(
        DebugCategory cat,
        DebugLevel lvl,
        const char* file,
        int line,
        const char* func,
        Args&&... args
    ) {
        if (lvl < level) return;
        if (!(categoryMask & (1u << int(cat)))) return;

        std::ostringstream oss;
        oss << "[" << std::left << std::setw(10) << categoryToStr(cat)
            << "][" << std::setw(5) << levelToStr(lvl)
            << "][" << std::setw(100) << (std::string(file) + ":" + std::to_string(line))
            << "][" << std::setw(12) << func << "] ";
        (oss << ... << std::forward<Args>(args)); // fold expression

        std::cerr << oss.str() << '\n';
    }

    template <typename... Args>
    [[noreturn]] static void fatal(
        DebugCategory cat,
        const char* file,
        int line,
        const char* func,
        Args&&... args
    ) {
        std::ostringstream oss;
        oss << "[" << std::left << std::setw(7) << categoryToStr(cat)
            << "][" << std::setw(20) << (std::string(file) + ":" + std::to_string(line))
            << "][" << std::setw(12) << func << "] ";
        (oss << ... << std::forward<Args>(args)); // fold expression

        std::cout << oss.str() << '\n';

        std::abort();
    }

private:
    static const char* levelToStr(DebugLevel lvl);
    static const char* categoryToStr(DebugCategory cat);
};

#define ENGINE_FATAL(cat, ...)\
    Debug::fatal(cat, __FILE__, __LINE__, __func__, __VA_ARGS__)

#if ENGINE_DEBUG
#define LOG_DEBUG(cat, ...) \
    Debug::logStream(cat, DebugLevel::DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_INFO(cat, ...) \
    Debug::logStream(cat, DebugLevel::INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_WARN(cat, ...) \
    Debug::logStream(cat, DebugLevel::WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_ERROR(cat, ...) \
    Debug::logStream(cat, DebugLevel::ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#endif
