#version 460

#include "../common/shader_common.glsl"

layout(location = 0) in Interpolants IN;

layout(location = 0) out vec4 outColor;

void main()
{
  vec3 color = IN.color.rgb;

  outColor = vec4(color, 1.0f);
}