// Dynamic lighting shared by the OpenGL and Vulkan fragment shaders.
//
// Vulkan shaders define SPEAR_VULKAN and SPEAR_LIGHTING_SET (descriptor set index) before including this.
// The LightingData block must match spear::rendering::LightingUniform.

#define SPEAR_MAX_POINT_LIGHTS 16
#define SPEAR_MAX_POINT_SHADOWS 4

struct SpearPointLight
{
    vec4 positionRange;  // xyz position, w range
    vec4 colorIntensity; // rgb color, a intensity
    ivec4 shadow;        // x shadow cube slot, -1 when not shadowed
};

#ifdef SPEAR_VULKAN
layout(set = SPEAR_LIGHTING_SET, binding = 0) uniform LightingData
#else
layout(std140, binding = 0) uniform LightingData
#endif
{
    mat4 inverseViewProjection;
    mat4 sunViewProjection;
    vec4 sunDirection;   // xyz towards the sun, w 1 when the sun casts shadows
    vec4 sunColor;       // rgb color, a intensity
    vec4 ambient;
    vec4 cameraPosition;
    vec4 viewport;
    ivec4 counts;        // x point light count
    vec4 shadowParams;   // x point shadow near plane, y sun shadow texel size
    SpearPointLight pointLights[SPEAR_MAX_POINT_LIGHTS];
} lighting;

#ifdef SPEAR_VULKAN
layout(set = SPEAR_LIGHTING_SET, binding = 1) uniform sampler2DShadow sunShadowMap;
layout(set = SPEAR_LIGHTING_SET, binding = 2) uniform samplerCubeShadow pointShadowMaps[SPEAR_MAX_POINT_SHADOWS];
#else
layout(binding = 1) uniform sampler2DShadow sunShadowMap;
layout(binding = 2) uniform samplerCubeShadow pointShadowMaps[SPEAR_MAX_POINT_SHADOWS];
#endif

// World position reconstructed from the depth buffer, so no vertex shader changes are needed.
vec3 spearWorldPosition()
{
    vec2 ndc = gl_FragCoord.xy / lighting.viewport.xy * 2.0 - 1.0;
#ifdef SPEAR_VULKAN
    vec4 world = lighting.inverseViewProjection * vec4(ndc.x, -ndc.y, gl_FragCoord.z, 1.0);
#else
    vec4 world = lighting.inverseViewProjection * vec4(ndc, gl_FragCoord.z * 2.0 - 1.0, 1.0);
#endif
    return world.xyz / world.w;
}

// Flat face normal, oriented towards the camera.
vec3 spearSurfaceNormal(vec3 world)
{
    vec3 normal = normalize(cross(dFdx(world), dFdy(world)));
    if (dot(normal, lighting.cameraPosition.xyz - world) < 0.0)
        normal = -normal;
    return normal;
}

float spearSunVisibility(vec3 world)
{
    if (lighting.sunDirection.w < 0.5)
        return 1.0;

    vec4 light = lighting.sunViewProjection * vec4(world, 1.0);
    vec3 projected = light.xyz / light.w;
#ifdef SPEAR_VULKAN
    vec2 uv = vec2(projected.x, -projected.y) * 0.5 + 0.5;
    float depth = projected.z;
#else
    vec2 uv = projected.xy * 0.5 + 0.5;
    float depth = projected.z * 0.5 + 0.5;
#endif
    if (any(lessThan(uv, vec2(0.0))) || any(greaterThan(uv, vec2(1.0))) || depth < 0.0 || depth > 1.0)
        return 1.0;

    float texel = lighting.shadowParams.y;
    float visibility = 0.0;
    for (int y = -1; y <= 1; ++y)
        for (int x = -1; x <= 1; ++x)
            visibility += texture(sunShadowMap, vec3(uv + vec2(x, y) * texel, depth - 0.00015));
    return visibility / 9.0;
}

// Converts a light-to-fragment vector into the depth the cube face stored for it.
float spearCubeDepth(vec3 toFragment, float far)
{
    vec3 a = abs(toFragment);
    float near = lighting.shadowParams.x;
    float d = max(a.x, max(a.y, a.z));
    d = max(d - max(0.02, d * 0.01), near); // Linear bias against acne.
#ifdef SPEAR_VULKAN
    return far * (d - near) / ((far - near) * d);
#else
    return ((far + near) - 2.0 * far * near / d) / (far - near) * 0.5 + 0.5;
#endif
}

float spearPointVisibility(int slot, vec3 toFragment, float range)
{
    float depth = spearCubeDepth(toFragment, range);
    float radius = length(toFragment) * 0.006;
    float visibility = texture(pointShadowMaps[slot], vec4(toFragment, depth));
    for (int i = 0; i < 8; ++i)
    {
        vec3 offset = vec3((i & 1) == 0 ? -1.0 : 1.0, (i & 2) == 0 ? -1.0 : 1.0, (i & 4) == 0 ? -1.0 : 1.0);
        visibility += texture(pointShadowMaps[slot], vec4(toFragment + offset * radius, depth));
    }
    return visibility / 9.0;
}

float spearAttenuation(float distance, float range)
{
    float falloff = clamp(1.0 - pow(distance / range, 4.0), 0.0, 1.0);
    return falloff * falloff / (1.0 + distance * distance);
}

// Total light reaching the current fragment: ambient + sun + point lights, with shadows.
vec3 spearLighting()
{
    vec3 world = spearWorldPosition();
    vec3 normal = spearSurfaceNormal(world);
    vec3 view = normalize(lighting.cameraPosition.xyz - world);
    vec3 result = lighting.ambient.rgb;

    if (lighting.sunColor.a > 0.0)
    {
        vec3 l = normalize(lighting.sunDirection.xyz);
        float diffuse = max(dot(normal, l), 0.0);
        if (diffuse > 0.0)
            result += lighting.sunColor.rgb * lighting.sunColor.a * diffuse * spearSunVisibility(world);
    }

    for (int i = 0; i < lighting.counts.x; ++i)
    {
        SpearPointLight light = lighting.pointLights[i];
        vec3 toFragment = world - light.positionRange.xyz;
        float distance = length(toFragment);
        if (distance >= light.positionRange.w)
            continue;

        vec3 l = -toFragment / max(distance, 1e-4);
        float diffuse = max(dot(normal, l), 0.0);
        if (diffuse <= 0.0)
            continue;

        float specular = pow(max(dot(normal, normalize(l + view)), 0.0), 32.0) * 0.25;
        float visibility = 1.0;
        if (light.shadow.x >= 0)
            visibility = spearPointVisibility(light.shadow.x, toFragment, light.positionRange.w);

        vec3 radiance = light.colorIntensity.rgb * light.colorIntensity.a *
                        spearAttenuation(distance, light.positionRange.w);
        result += radiance * (diffuse + specular) * visibility;
    }

    return result;
}
