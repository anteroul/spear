#version 450

// Soft, lit smoke puff. Shape comes from procedural noise, edges fade against the scene depth,
// and lighting reuses the sun and point light data (including shadows) from lighting_common.glsl.

#define SPEAR_VULKAN
#define SPEAR_LIGHTING_SET 0
#include "lighting_common.glsl"

layout(input_attachment_index = 0, set = 1, binding = 2) uniform subpassInput sceneDepth;

layout(push_constant) uniform PushConstants
{
    mat4 viewProjection;
    vec4 cameraRight;
    vec4 cameraUp;
} pc;

layout(location = 0) in vec3 fragWorld;
layout(location = 1) in vec2 fragCorner;
layout(location = 2) in vec2 fragFacing;
layout(location = 3) in vec4 fragColor;
layout(location = 4) in vec3 fragParams;

layout(location = 0) out vec4 outColor;

float hash(vec3 p)
{
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float valueNoise(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), hash(i + vec3(1, 0, 0)), f.x),
                   mix(hash(i + vec3(0, 1, 0)), hash(i + vec3(1, 1, 0)), f.x), f.y),
               mix(mix(hash(i + vec3(0, 0, 1)), hash(i + vec3(1, 0, 1)), f.x),
                   mix(hash(i + vec3(0, 1, 1)), hash(i + vec3(1, 1, 1)), f.x), f.y),
               f.z);
}

float fbm(vec3 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; ++i)
    {
        value += amplitude * valueNoise(p);
        p = p * 2.03 + vec3(1.7, 9.2, 4.1);
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    float r2 = dot(fragCorner, fragCorner);
    if (r2 >= 1.0)
        discard;

    float size = fragParams.x;
    float life = fragParams.y;
    float seed = fragParams.z;

    // Puff shape: round falloff eroded by noise that slowly churns over the particle's life.
    float shape = 1.0 - sqrt(r2);
    float noise = fbm(vec3(fragCorner * 2.2 + seed * 37.0, seed * 11.0 + life * 1.5));
    float density = shape * smoothstep(0.3, 0.8, noise + shape * 0.35);

    // Soft particles: fade out where the quad gets close to the opaque scene behind it.
    vec3 camera = lighting.cameraPosition.xyz;
    vec2 ndc = gl_FragCoord.xy / lighting.viewport.xy * 2.0 - 1.0;
    vec4 scene = lighting.inverseViewProjection * vec4(ndc.x, -ndc.y, subpassLoad(sceneDepth).r, 1.0);
    float sceneDistance = distance(scene.xyz / scene.w, camera);
    float fragmentDistance = distance(fragWorld, camera);
    float soft = clamp((sceneDistance - fragmentDistance) / max(0.25, size * 0.5), 0.0, 1.0);
    float nearFade = clamp((fragmentDistance - 0.3) / 1.0, 0.0, 1.0);

    float alpha = density * fragColor.a * soft * nearFade;
    if (alpha < 0.003)
        discard;

    // Treat the puff as a sphere so light wraps around it instead of hitting a flat card.
    vec3 toCamera = normalize(camera - fragWorld);
    vec3 normal = normalize(pc.cameraRight.xyz * fragFacing.x + pc.cameraUp.xyz * fragFacing.y +
                            toCamera * sqrt(1.0 - r2));

    vec3 light = lighting.ambient.rgb;
    if (lighting.sunColor.a > 0.0)
    {
        float wrap = clamp(dot(normal, normalize(lighting.sunDirection.xyz)) * 0.5 + 0.5, 0.0, 1.0);
        light += lighting.sunColor.rgb * lighting.sunColor.a * wrap * spearSunVisibility(fragWorld);
    }
    for (int i = 0; i < lighting.counts.x; ++i)
    {
        SpearPointLight point = lighting.pointLights[i];
        vec3 toFragment = fragWorld - point.positionRange.xyz;
        float distance = length(toFragment);
        if (distance >= point.positionRange.w)
            continue;

        float wrap = clamp(dot(normal, -toFragment / max(distance, 1e-4)) * 0.5 + 0.5, 0.0, 1.0);
        float visibility = 1.0;
        if (point.shadow.x >= 0)
            visibility = spearPointVisibility(point.shadow.x, toFragment, point.positionRange.w);
        light += point.colorIntensity.rgb * point.colorIntensity.a *
                 spearAttenuation(distance, point.positionRange.w) * wrap * visibility;
    }

    // Premultiplied alpha.
    outColor = vec4(fragColor.rgb * light * alpha, alpha);
}
