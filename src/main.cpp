#include <iostream>
#include "core/loader.hpp"

modLoader loader;

void movWriteHeadToTopLeft()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hConsole, pos);
}  

bool options()
{
    movWriteHeadToTopLeft();
    std::string option = "Options:\n";
    "I - Invoke mods\n";
    "D - Invoke mod destructors\n";
    "Q - Quit\n";
    std::cout << option;
    std::string input;
    std::cin >> input;
    if (input == "I")
    {
        loader.InvokeMain();
        return true;
    }
    else if (input == "D")
    {
        loader.InvokeDestructor();
        return true;
    }
    else if (input == "Q")
    {
        exit(0);
        return false;
    }
    else
    {
        std::cout << "Invalid option!\n";
        return options();
    }
}

int main()
{
    // get the current working directory
    char buffer[32767]; // we cant use MAX_PATH because of the long path option that windows has
    GetCurrentDirectoryA(MAX_PATH, buffer);
    loader.Load(buffer + std::string("\\mods"));
    bool loop = true;
    while (loop) {
        loop = options();
    }
}
