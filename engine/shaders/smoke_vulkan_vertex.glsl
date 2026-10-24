#version 450

// Expands each smoke particle into a camera facing quad. No vertex buffer: six vertices per particle.

#include "smoke_common.glsl"

layout(std430, set = 1, binding = 0) readonly buffer Particles
{
    SmokeParticle particles[];
};

layout(push_constant) uniform PushConstants
{
    mat4 viewProjection;
    vec4 cameraRight;
    vec4 cameraUp;
} pc;

layout(location = 0) out vec3 fragWorld;
layout(location = 1) out vec2 fragCorner;  // Quad corner in [-1, 1], rotates with the particle.
layout(location = 2) out vec2 fragFacing;  // Corner in camera space, for the spherical normal.
layout(location = 3) out vec4 fragColor;   // rgb color, a opacity including the lifetime fade
layout(location = 4) out vec3 fragParams;  // x half size, y life fraction, z seed

const vec2 corners[6] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
                               vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0));

void main()
{
    SmokeParticle p = particles[gl_VertexIndex / 6];
    float age = p.positionAge.w;
    if (p.misc.w < 0.5 || age < 0.0)
    {
        // Every vertex of a hidden particle lands on the same point, so the quad has no area.
        gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
        return;
    }

    vec2 corner = corners[gl_VertexIndex % 6];
    float s = sin(p.misc.y);
    float c = cos(p.misc.y);
    vec2 facing = mat2(c, s, -s, c) * corner;

    float size = p.misc.x;
    vec3 world = p.positionAge.xyz + (pc.cameraRight.xyz * facing.x + pc.cameraUp.xyz * facing.y) * size;

    float t = clamp(age / p.velocityLife.w, 0.0, 1.0);
    float fade = smoothstep(0.0, 0.08, t) * (1.0 - smoothstep(0.5, 1.0, t));

    fragWorld = world;
    fragCorner = corner;
    fragFacing = facing;
    fragColor = vec4(p.colorOpacity.rgb, p.colorOpacity.a * fade);
    fragParams = vec3(size, t, p.misc.z);

    gl_Position = pc.viewProjection * vec4(world, 1.0);
    gl_Position.y = -gl_Position.y; // Vulkan Y-axis flip
}
