#version 450

#define SPEAR_VULKAN
#define SPEAR_LIGHTING_SET 0
#include "lighting_common.glsl"

layout(location = 0) in vec3 fragColor;
layout(push_constant) uniform PushConstants {
    mat4 mvp;
    vec4 emission;
} pc;

layout(location = 0) out vec4 outColor;

void main()
{
    if (pc.emission.x > 0.5) {
        outColor = vec4(1.0, 0.88, 0.45, 1.0);
        return;
    }
    outColor = vec4(fragColor * spearLighting(), 1.0);
}
