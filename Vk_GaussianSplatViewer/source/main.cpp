
#include <iostream>
#include "core/Engine.h"
#include "3d/GaussianSplatPlyLoader.h"

int main()
{
    splat_loader::GaussianSplatPlyLoader ply;

    if (!ply.load(R"(D:\Projects\CPP\Vk_GaussianSplat\data\truck_point_cloud.ply)"))
    {
        std::cerr << "Failed to load PLY\n";
        return -1;
    }

    const auto& gaussians = ply.get_gaussians();

    std::cout << "Loaded " << gaussians.size() << " gaussians\n";

    int x = 0;
    std::cin >> x;

    core::Engine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    
    return 0;
}
