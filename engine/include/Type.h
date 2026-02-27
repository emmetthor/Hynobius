#pragma once
#include <cstdint>

enum class Player
{
    WHITE,
    BLACK
};

enum class EVALUATE_MODE
{
    QUICK,
    POSITION,
    FULL
};

using Square = uint8_t;
using BitMove = uint32_t;