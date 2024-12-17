#version 450 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec4 color;

void main()
{
    FragColor = texture(textureSampler, uv) * color;
}
