#version 450

#define SPEAR_VULKAN
#define SPEAR_LIGHTING_SET 1
#include "lighting_common.glsl"

layout(set = 0, binding = 0) uniform sampler2D texSampler;
layout(push_constant) uniform PushConstants {
    mat4 mvp;
    vec4 viewModel;
} pc;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main()
{
    if (pc.viewModel.x > 0.5) {
        outColor = texture(texSampler, fragUV);
        return;
    }
    vec4 base = texture(texSampler, fragUV);
    outColor = vec4(base.rgb * spearLighting(), base.a);
}
