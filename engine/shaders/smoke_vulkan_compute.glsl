#version 450

// Moves every smoke particle one frame forward. Each emitter owns a fixed block of particle
// slots; a slot that dies respawns at its emitter, so the emission rate is count / lifetime.

#include "smoke_common.glsl"

layout(local_size_x = 64) in;

struct SmokeEmitterData
{
    vec4 positionRadius;   // xyz position, w spawn radius
    vec4 velocityLifetime; // xyz initial velocity, w lifetime
    vec4 colorOpacity;     // rgb color, a opacity
    vec4 shape;            // x start size, y end size, z turbulence, w active particle count
};

layout(std430, set = 0, binding = 0) buffer Particles
{
    SmokeParticle particles[];
};

layout(std140, set = 0, binding = 1) uniform SmokeData
{
    vec4 params; // x delta time, y time, z emitter count
    vec4 wind;
    SmokeEmitterData emitters[SPEAR_SMOKE_MAX_EMITTERS];
} smoke;

uint hash(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

float random(inout uint state)
{
    state = hash(state);
    return float(state) * (1.0 / 4294967295.0);
}

// Smooth swirling velocity field. Cheap stand-in for curl noise.
vec3 flow(vec3 p, float t)
{
    return vec3(sin(p.y * 1.3 + t * 0.7) + sin(p.z * 0.9 - t * 0.4),
                0.35 * sin(p.x * 0.8 + p.z * 0.6 + t * 0.5),
                cos(p.y * 1.1 - t * 0.6) + cos(p.x * 0.7 + t * 0.3)) * 0.5;
}

void place(inout SmokeParticle p, SmokeEmitterData e, inout uint state, float age)
{
    float angle = random(state) * 6.2831853;
    float radius = e.positionRadius.w * sqrt(random(state));
    vec3 offset = vec3(cos(angle), 0.0, sin(angle)) * radius;

    vec3 jitter = vec3(random(state), random(state), random(state)) * 2.0 - 1.0;
    float speed = length(e.velocityLifetime.xyz);

    p.positionAge = vec4(e.positionRadius.xyz + offset, age);
    p.velocityLife = vec4(e.velocityLifetime.xyz + jitter * speed * 0.25,
                          e.velocityLifetime.w * (0.8 + 0.4 * random(state)));
    p.colorOpacity = e.colorOpacity;
    p.misc = vec4(e.shape.x, random(state) * 6.2831853, random(state), 1.0);
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if (id >= particles.length())
        return;

    uint emitterIndex = id / SPEAR_SMOKE_PARTICLES_PER_EMITTER;
    uint slot = id % SPEAR_SMOKE_PARTICLES_PER_EMITTER;
    SmokeEmitterData e = smoke.emitters[emitterIndex];
    bool emitting = emitterIndex < uint(smoke.params.z) && float(slot) < e.shape.w;

    float dt = smoke.params.x;
    float time = smoke.params.y;
    uint state = hash(id * 1973u + floatBitsToUint(time));
    SmokeParticle p = particles[id];

    if (p.misc.w < 0.5)
    {
        // Dead slot: wait a random part of a lifetime before the first spawn so emission stays even.
        if (emitting)
        {
            p.misc = vec4(e.shape.x, 0.0, 0.0, 1.0);
            p.velocityLife.w = e.velocityLifetime.w;
            p.positionAge = vec4(e.positionRadius.xyz, -random(state) * e.velocityLifetime.w);
            p.colorOpacity = vec4(0.0);
            particles[id] = p;
        }
        return;
    }

    float previousAge = p.positionAge.w;
    float age = previousAge + dt;
    if (age < 0.0)
    {
        p.positionAge.w = age;
        particles[id] = p;
        return;
    }
    if (previousAge < 0.0)
    {
        place(p, e, state, age);
    }
    else if (age >= p.velocityLife.w)
    {
        if (emitting)
            place(p, e, state, min(age - p.velocityLife.w, dt));
        else
            p.misc.w = 0.0;
        particles[id] = p;
        return;
    }

    float t = clamp(age / p.velocityLife.w, 0.0, 1.0);
    vec3 position = p.positionAge.xyz;
    vec3 velocity = p.velocityLife.xyz;

    // Buoyancy, wind and swirl, with drag so particles slow down and spread out as they rise.
    vec3 acceleration = flow(position * 0.5, time) * e.shape.z + smoke.wind.xyz + vec3(0.0, 0.15, 0.0);
    velocity += acceleration * dt;
    velocity *= exp(-0.45 * dt);
    position += velocity * dt;

    p.positionAge = vec4(position, age);
    p.velocityLife.xyz = velocity;
    // Particles of a removed emitter keep their last size until they fade out.
    if (emitterIndex < uint(smoke.params.z))
        p.misc.x = mix(e.shape.x, e.shape.y, sqrt(t));
    p.misc.y += (p.misc.z - 0.5) * 0.8 * dt;
    particles[id] = p;
}
