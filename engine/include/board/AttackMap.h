#pragma once

class AttackMap
{
private:
    int attackMap[2][8][8];

public:
    void clear();
    void calculate(const Board& board);
    bool isSquareAttacked(Position pos, const Player player);
    int countSquareAttacks(Position pos, const Player player);
    void debugPrint();
};
