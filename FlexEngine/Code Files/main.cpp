#include <iostream>

#include "FlexEngine.h"


int main()
{
    FXE::FlexEngine Engine;

    try 
    {
        Engine.init_Engine();
        Engine.game_Tick();
        Engine.cleanup_Engine();
    }

    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}