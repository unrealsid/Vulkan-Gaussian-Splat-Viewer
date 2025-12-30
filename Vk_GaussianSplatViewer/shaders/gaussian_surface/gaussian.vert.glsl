#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_SH;
layout (location = 3) in float in_opacity;
layout (location = 4) in vec3 in_scale;
layout (location = 5) in vec4 in_rotation;

layout (location = 0) out vec3 fragColor;

layout(buffer_reference, std430) readonly buffer CameraData
{
	mat4 projection;
	mat4 view;
};

//layout(buffer_reference, scalar) buffer ModelTransform
//{
//	mat4 model_transform;
//};

layout(push_constant) uniform PushConstants
{
	CameraData camera_data_adddress;
	//ModelTransform model_transform_address;
} pc;

void main()
{
	CameraData matrices = CameraData(pc.camera_data_adddress);
	//ModelTransform model_transform = ModelTransform(pc.model_transform_address);

	vec4 position = vec4(in_position, 1.0);
	//Flipping for getting scene right
	position.xy *= -1.0;

	gl_Position = matrices.projection * matrices.view * position;
	gl_PointSize = 20.0;

	fragColor = in_SH * 0.28 + vec3(0.5, 0.5, 0.5);
}