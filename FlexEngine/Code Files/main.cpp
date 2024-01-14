#include <iostream>

#include "EngineApplication.h"


int main()
{
    FlexEngine app;

    try 
    {
        app.run();
    }

    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
   
    return EXIT_SUCCESS;
}