#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

struct Interpolants
{
    vec3  position;     // World-space vertex position
    float depth;   // Z coordinate after applying the view matrix (larger = further away)
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer CameraData
{
    mat4 projection;
    mat4 view;
    vec4 camera_position;
};

layout(push_constant) uniform PushConstants
{
    CameraData camera_data_adddress;
} pc;
