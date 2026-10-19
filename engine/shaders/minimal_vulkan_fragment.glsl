#version 450

layout(location = 0) in vec3 fragColor;
layout(push_constant) uniform PushConstants {
    mat4 mvp;
    vec4 emission;
} pc;
layout(set = 0, binding = 0) uniform SunData {
    mat4 inverseViewProjection;
    mat4 lightViewProjection;
    vec4 direction;
    vec4 viewport;
} sun;
layout(set = 0, binding = 1) uniform sampler2DShadow shadowMap;

layout(location = 0) out vec4 outColor;

void main()
{
    if (pc.emission.x > 0.5) {
        outColor = vec4(1.0, 0.88, 0.45, 1.0);
        return;
    }
    vec2 ndc = gl_FragCoord.xy / sun.viewport.xy * 2.0 - 1.0;
    vec4 world = sun.inverseViewProjection * vec4(ndc.x, -ndc.y, gl_FragCoord.z, 1.0);
    world /= world.w;
    vec3 normal = normalize(cross(dFdx(world.xyz), dFdy(world.xyz)));
    float diffuse = abs(dot(normal, sun.direction.xyz));
    vec4 light = sun.lightViewProjection * world;
    vec3 projected = light.xyz / light.w;
    vec2 uv = vec2(projected.x, -projected.y) * 0.5 + 0.5;
    float visibility = 1.0;
    if (all(greaterThanEqual(uv, vec2(0.0))) && all(lessThanEqual(uv, vec2(1.0))) &&
        projected.z >= 0.0 && projected.z <= 1.0) {
        visibility = 0.0;
        for (int y = -1; y <= 1; ++y)
            for (int x = -1; x <= 1; ++x)
                visibility += texture(shadowMap, vec3(uv + vec2(x, y) / 2048.0,
                                                     projected.z - 0.00015));
        visibility /= 9.0;
    }
    float lighting = 0.45 + 0.55 * diffuse * visibility;
    outColor = vec4(fragColor * lighting, 1.0);
}
