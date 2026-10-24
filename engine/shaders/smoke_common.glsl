// Particle and emitter layouts shared by the smoke compute, vertex and fragment shaders.
// SmokeParticle must match spear::rendering::vulkan::SmokeRenderer's particle size (64 bytes).

#define SPEAR_SMOKE_MAX_EMITTERS 8
#define SPEAR_SMOKE_PARTICLES_PER_EMITTER 512

struct SmokeParticle
{
    vec4 positionAge;   // xyz world position, w age in seconds (negative while waiting to spawn)
    vec4 velocityLife;  // xyz velocity, w lifetime in seconds
    vec4 colorOpacity;  // rgb color, a opacity
    vec4 misc;          // x half size, y rotation, z random seed, w 1 when alive
};
