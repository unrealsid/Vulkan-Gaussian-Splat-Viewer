#version 460

#include "../common/shader_common.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out Interpolants OUT;

void main()
{
  CameraData matrices = CameraData(pc.camera_data_adddress);

  gl_Position = matrices.projection * matrices.view * vec4(inPosition, 1.0);
  OUT.depth   = (matrices.view * vec4(inPosition, 1.0)).z;
  OUT.position = inPosition;
  OUT.color = inColor;
}