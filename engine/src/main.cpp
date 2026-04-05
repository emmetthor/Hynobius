#include "Engine.h"
#include "UCI/UCI.h"

int main()
{
    Engine engine;

    uciLoop(engine);

    return 0;
}