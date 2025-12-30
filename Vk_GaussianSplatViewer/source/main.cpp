#include "3d/ModelUtils.h"
#include "core/Engine.h"
#include <iostream>

int main()
{
    std::cout << "Loading Gaussian Splat..." << "\n";
    auto gaussians = entity_3d::ModelUtils::load_gaussian_surfaces(R"(D:\Projects\CPP\Vk_GaussianSplat\data\truck_point_cloud.ply)");; //entity_3d::ModelUtils::load_test_gaussian_model();

    core::Engine engine;
    engine.init();
    engine.gaussian_surface_init(gaussians);

    engine.run();
    engine.cleanup();
    
    return 0;
}
