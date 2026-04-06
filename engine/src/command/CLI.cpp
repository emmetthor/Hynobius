#include "command/CLI.h"
#include "command/UCI.h"
#include <sstream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#define END     RESET << '\n' << std::flush

void handlePerft(std::istringstream iss, Engine &engine) {
    
}

void cliLoop(Engine &engine)
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci")
        {
            uciLoop(engine);
            std::cout << "mode back to" << GREEN << "CLI" << END;
        }
        else if (token == "test")
        {
            std::cout << BLUE << "test blue" << END;
        }
        else if (token == "perft")
        {

        }
        else
        {
            std::cerr << "Unrecognized token.\n" << END;
        }
    }
}