#version 450 core

#include "lighting_common.glsl"

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textureSampler;

void main()
{
    vec4 base = texture(textureSampler, TexCoords);
    FragColor = vec4(base.rgb * spearLighting(), base.a);
}
