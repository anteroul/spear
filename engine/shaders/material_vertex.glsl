#version 450 core

// Vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 mvp;

// Outputs to fragment shader
out vec3 fragNormal;      // Normal vector in world space
out vec2 fragTexCoord;    // Texture coordinates

void main()
{
    // Pass through the texture coordinates
    fragTexCoord = aTexCoord;

    // Transform normal from object space to world space using the model matrix
    fragNormal = aNormal;

    // Transform the vertex position to clip space using the MVP matrix
    gl_Position = mvp * vec4(aPosition, 1.0);
}
