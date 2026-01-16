#pragma once
#include <vector>

#include "VkBootstrapDispatch.h"
#include <vulkan/vulkan_core.h>

struct EngineContext;

namespace renderer
{
    class DrawState
    {
    public:
       DrawState(EngineContext& context);
        ~DrawState() = default;

        // Combined set and apply methods
        void set_and_apply_viewport_scissor(VkCommandBuffer cmd_buffer, VkExtent2D viewport_extent,
                                            VkExtent2D scissor_extents, VkOffset2D scissor_offset);

        void set_and_apply_vertex_input(VkCommandBuffer cmd_buffer,
                                        const std::vector<VkVertexInputBindingDescription2EXT>& binding,
                                        const std::vector<VkVertexInputAttributeDescription2EXT>& attributes);

        void set_and_apply_color_blend(VkCommandBuffer cmd_buffer,
                                       const std::vector<VkColorComponentFlags>& flags,
                                       const std::vector<VkBool32>& enables);

        void set_and_apply_blend_equation(VkCommandBuffer cmd_buffer,
                                          VkBlendFactor src_color_blend_factor,
                                          VkBlendFactor dst_color_blend_factor,
                                          VkBlendOp color_blend_op,
                                          VkBlendFactor src_alpha_blend_factor,
                                          VkBlendFactor dst_alpha_blend_factor,
                                          VkBlendOp alpha_blend_op);

        // Static state application (doesn't need setting)
        void apply_rasterization_state(VkCommandBuffer cmd_buffer) const;
        void apply_depth_stencil_state(VkCommandBuffer cmd_buffer) const;

        // Apply all states at once
        void apply_all_draw_states(VkCommandBuffer cmd_buffer) const;

    private:
        VkExtent2D viewport_extent{};
        VkExtent2D scissor_extents{};
        VkOffset2D scissor_offset{};
        std::vector<VkVertexInputBindingDescription2EXT> vertex_input_binding;
        std::vector<VkVertexInputAttributeDescription2EXT> input_attribute_description;
        std::vector<VkColorComponentFlags> color_component_flags;
        std::vector<VkBool32> color_blend_enables;
        VkColorBlendEquationEXT color_blend_equation{};

        EngineContext& engine_context;
    };
}