#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "WackyEngine/Application.h"

int main() 
{
    WackyEngine::Application app(1280, 720, "Test Application");

    try
    {
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}