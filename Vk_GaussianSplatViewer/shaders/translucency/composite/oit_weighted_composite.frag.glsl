#version 460


#include "../../common/shader_common.glsl"

layout(input_attachment_index = 0, binding = 0) uniform subpassInput texColor;
layout(input_attachment_index = 1, binding = 1) uniform subpassInput texWeights;

layout(location = 0) out vec4 outColor;

void main()
{
  vec4 accum = subpassLoad(texColor);
  float reveal = subpassLoad(texWeights).r;

  // GL blend function: GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA
  outColor = vec4(accum.rgb / max(accum.a, 1e-5), reveal);
}