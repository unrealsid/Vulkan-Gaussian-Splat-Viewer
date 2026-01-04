#include "3d/ModelUtils.h"
#include "core/Engine.h"
#include <iostream>

int main()
{
    core::Engine engine;
    engine.init();

    engine.run();
    engine.cleanup();
    
    return 0;
}
