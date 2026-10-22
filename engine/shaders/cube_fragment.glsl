#version 450 core

#include "lighting_common.glsl"

in vec2 uv;
out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec4 color;

void main()
{
    vec4 base = texture(textureSampler, uv) * color;
    FragColor = vec4(base.rgb * spearLighting(), base.a);
}
