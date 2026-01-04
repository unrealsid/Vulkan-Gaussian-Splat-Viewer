#pragma once

#include "camera/FirstPersonCamera.h"
#include "renderer/Subpass.h"
#include "structs/geometry/GaussianSurface.h"
#include "structs/scene/CameraData.h"
#include "structs/scene/GeometryData.h"
#include "structs/scene/CommonSceneData.h"


namespace core::renderer
{
    class GeometryPass : public Subpass
    {

    public:
        GeometryPass(EngineContext& engine_context, uint32_t max_frames_in_flight, CommonSceneData& common_scene_data);
        void record_commands(VkCommandBuffer* command_buffer, uint32_t image_index, bool is_last) override;

        void allocate_gaussian_surface_buffer(const std::vector<GaussianSurface>& gaussians) const;

        void cleanup() override;

    private:
        std::unique_ptr<GaussianGeometryData> geometry_data;

        CommonSceneData& common_scene_data;

        camera::FirstPersonCamera* camera;
        VkExtent2D extents{};
        CameraData camera_data{};
    };
}
