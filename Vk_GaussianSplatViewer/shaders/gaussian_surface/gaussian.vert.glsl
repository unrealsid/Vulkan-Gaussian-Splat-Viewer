#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout (location = 0) in vec4 in_bounding_cube_position;

layout (location = 0) out vec3 frag_color;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 ellipsoidCenter;
layout (location = 3) out vec3 ellipsoidScale;
layout (location = 4) out mat3 ellipsoidRot;
layout (location = 7) out float ellipsoidAlpha;

mat3 quatToMat(vec4 q) {
	return mat3(2.0 * (q.x * q.x + q.y * q.y) - 1.0, 2.0 * (q.y * q.z + q.x * q.w), 2.0 * (q.y * q.w - q.x * q.z), // 1st column
	2.0 * (q.y * q.z - q.x * q.w), 2.0 * (q.x * q.x + q.z * q.z) - 1.0, 2.0 * (q.z * q.w + q.x * q.y), // 2nd column
	2.0 * (q.y * q.w + q.x * q.z), 2.0 * (q.z * q.w - q.x * q.y), 2.0 * (q.x * q.x + q.w * q.w) - 1.0); // last column
}

layout(buffer_reference, std430) readonly buffer CameraData
{
	mat4 projection;
	mat4 view;
	vec4 camera_position;
};

layout(buffer_reference, std430) readonly buffer SplatPosition
{
	vec4 positions[];
};

layout(buffer_reference, std430) readonly buffer SplatScale
{
	vec4 scales[];
};

layout(buffer_reference, std430) readonly buffer SplatColor
{
	vec4 colors[];
};

layout(buffer_reference, std430) readonly buffer SplatQuat
{
	vec4 quats[];
};

layout(buffer_reference, std430) readonly buffer SplatAlpha
{
	float alphas[];
};

layout(push_constant) uniform PushConstants
{
	CameraData camera_data_adddress;
	SplatPosition splat_positions_address;
	SplatScale splat_scales_address;
	SplatColor splat_colors_address;
	SplatQuat splat_quats_address;
	SplatAlpha splat_alphas_address;
} pc;

void main()
{
	CameraData matrices = CameraData(pc.camera_data_adddress);
	SplatPosition splat_positions = pc.splat_positions_address;
	SplatScale splat_scale = pc.splat_scales_address;
	SplatColor splat_color = pc.splat_colors_address;
	SplatQuat splat_quat = pc.splat_quats_address;
	SplatAlpha splat_alpha = pc.splat_alphas_address;

	/* Base cube vertex */
	vec3 aPos = in_bounding_cube_position.xyz;

	/* #1: scale the input vertices */
	vec3 scale = splat_scale.scales[gl_InstanceIndex].xyz;
	vec3 scaled = scale * aPos;

	/* #2: transform the quaternions into rotation matrices */
	mat3 rot = quatToMat(splat_quat.quats[gl_InstanceIndex]);
	vec3 rotated = rot * scaled;

	/* #3: translate the vertices */
	vec3 posOffset = rotated + splat_positions.positions[gl_InstanceIndex].xyz;
	vec4 mPos = vec4(posOffset, 1.0);

	//mPos.xy *= -1.0;

	/* #4: pass the ellipsoid parameters to the fragment shader */
	position        = posOffset;
	ellipsoidCenter = splat_positions.positions[gl_InstanceIndex].xyz;
	ellipsoidScale  = scale;
	ellipsoidRot    = rot;
	ellipsoidAlpha  = splat_alpha.alphas[gl_InstanceIndex];

	/* Color */
	frag_color = splat_color.colors[gl_InstanceIndex].rgb * 0.28 + vec3(0.5);

	/* Final transform */
	gl_Position = matrices.projection * matrices.view * mPos;
}