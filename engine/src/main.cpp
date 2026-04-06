#include "Engine.h"
#include "command/CLI.h"

int main()
{
    Engine engine;

    cliLoop(engine);

    return 0;
}