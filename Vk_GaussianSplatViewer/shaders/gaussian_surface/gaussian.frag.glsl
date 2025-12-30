#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout (location = 0) in vec3 fragColor;

layout (location = 0) out vec4 outColor;

layout(buffer_reference, std430) readonly buffer CameraData
{
    mat4 projection;
    mat4 view;
};

layout(push_constant) uniform PushConstants
{
    CameraData camera_data_adddress;
//ModelTransform model_transform_address;
} pc;


void main () 
{
    const float radius = 0.25;
    if (length(gl_PointCoord - vec2(0.5)) > radius)
    {
        discard;
    }

    outColor = vec4(fragColor, 1.0);
}