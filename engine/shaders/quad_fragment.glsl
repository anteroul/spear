#version 450 core

#include "lighting_common.glsl"

in vec3 fragColor;
out vec4 color;

void main()
{
    color = vec4(fragColor * spearLighting(), 1.0);
}
