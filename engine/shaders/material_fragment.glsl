#version 450 core

#include "lighting_common.glsl"

in vec3 fragNormal;     // Interpolated normal vector
in vec2 fragTexCoord;   // Interpolated texture coordinates

// Uniforms
layout(binding = 0) uniform sampler2D textureSampler;

struct Material {
    float specularExponent; // Ns
    float dissolve;         // d
    int illuminationModel;  // illum
    vec3 ambientColor;      // Ka
    vec3 diffuseColor;      // Kd
    vec3 specularColor;     // Ks
};
uniform Material material;

// Output color
out vec4 color;

void main()
{
    vec3 base = texture(textureSampler, fragTexCoord).rgb * material.diffuseColor;
    color = vec4(base * spearLighting(), 1.0);
}
