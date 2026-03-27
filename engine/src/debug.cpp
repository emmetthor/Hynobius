#include "debug.h"

// 靜態成員要定義一次
DebugLevel Debug::level = DebugLevel::DEBUG;
uint32_t Debug::categoryMask = 0xFFFFFFFF;

// 非 template 函式定義
const char* Debug::levelToStr(DebugLevel lvl)
{
    switch (lvl)
    {
        case DebugLevel::INFO:
            return "INFO";
        case DebugLevel::DEBUG:
            return "DEBUG";
        case DebugLevel::WARN:
            return "WARN";
        case DebugLevel::ERROR:
            return "ERROR";
    }
    return "UNK";
}

const char* Debug::categoryToStr(DebugCategory cat)
{
    switch (cat)
    {
        case DebugCategory::TT:
            return "TT";
        case DebugCategory::MOVE:
            return "MOVE";
        case DebugCategory::QS:
            return "QS";
        case DebugCategory::EVAL:
            return "EVAL";
        case DebugCategory::BOARD:
            return "BOARD";
        case DebugCategory::SEARCH:
            return "SEARCH";
        case DebugCategory::ATK:
            return "ATK";
        case DebugCategory::GENERATE:
            return "GENERATE";
        case DebugCategory::PIECE:
            return "PIECE";
    }
    return "UNK";
}
