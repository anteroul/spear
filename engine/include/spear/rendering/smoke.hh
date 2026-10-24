#ifndef SPEAR_RENDERING_SMOKE_HH
#define SPEAR_RENDERING_SMOKE_HH

#include <glm/vec3.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace spear::rendering
{

/// Continuous source of smoke particles. Particles live in world space, so a moving emitter leaves a trail.
struct SmokeEmitter
{
    glm::vec3 position = glm::vec3(0.0f);
    /// Particles spawn on a horizontal disc of this radius around the position.
    float radius = 0.3f;

    /// Initial particle velocity in units per second.
    glm::vec3 velocity = glm::vec3(0.0f, 1.2f, 0.0f);
    /// Seconds a particle lives before it respawns at the emitter.
    float lifetime = 6.0f;

    glm::vec3 color = glm::vec3(0.55f);
    float opacity = 0.35f;

    /// Billboard half size at spawn and at the end of the particle's life.
    float startSize = 0.4f;
    float endSize = 2.5f;

    /// Particles emitted per second. Limited to SmokeManager::maxParticlesPerEmitter alive at once.
    float rate = 40.0f;
    /// Strength of the swirling flow field.
    float turbulence = 0.6f;

    bool enabled = true;
};

class SmokeManager
{
public:
    using Id = uint32_t;

    /// Emitters beyond these limits are ignored by the renderers.
    static constexpr size_t maxEmitters = 8;
    static constexpr size_t maxParticlesPerEmitter = 512;

    Id addEmitter(const SmokeEmitter& emitter);
    bool removeEmitter(Id id);
    void clearEmitters();

    /// Returns nullptr for unknown ids. The pointer is invalidated by add/remove.
    SmokeEmitter* getEmitter(Id id);

    const std::vector<std::pair<Id, SmokeEmitter>>& getEmitters() const
    {
        return m_emitters;
    }

    /// Wind applied to every particle, in units per second squared.
    void setWind(const glm::vec3& wind)
    {
        m_wind = wind;
    }
    const glm::vec3& getWind() const
    {
        return m_wind;
    }

private:
    std::vector<std::pair<Id, SmokeEmitter>> m_emitters;
    glm::vec3 m_wind = glm::vec3(0.0f);
    Id m_nextId = 1;
};

} // namespace spear::rendering

#endif
