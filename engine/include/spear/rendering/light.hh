#ifndef SPEAR_RENDERING_LIGHT_HH
#define SPEAR_RENDERING_LIGHT_HH

#include <glm/vec3.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace spear::rendering
{

/// Direction towards the sun for the default slow day orbit at the given time.
glm::vec3 sunDirectionAt(float seconds);

/// Omnidirectional light. Shadows are rendered into a cube map when castsShadow is set.
struct PointLight
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 10.0f;

    /// Distance at which the light fades out completely. Also the far plane of its shadow map.
    float range = 20.0f;

    bool castsShadow = false;
    bool enabled = true;
};

/// Infinitely distant light with an orthographic shadow map.
struct DirectionalLight
{
    /// Direction pointing towards the light.
    glm::vec3 direction = sunDirectionAt(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 0.55f;
    bool castsShadow = true;

    /// World space area covered by the shadow map.
    glm::vec3 shadowCenter = glm::vec3(-320.0f, 100.0f, -1120.0f);
    float shadowExtent = 4200.0f;
    float shadowDistance = 6000.0f;
    float shadowFar = 14000.0f;
};

class LightManager
{
public:
    using Id = uint32_t;

    /// Point lights beyond these limits are ignored by the renderers.
    static constexpr size_t maxPointLights = 16;
    static constexpr size_t maxShadowedPointLights = 4;

    Id addPointLight(const PointLight& light);
    bool removePointLight(Id id);
    void clearPointLights();

    /// Returns nullptr for unknown ids. The pointer is invalidated by add/remove.
    PointLight* getPointLight(Id id);

    const std::vector<std::pair<Id, PointLight>>& getPointLights() const
    {
        return m_pointLights;
    }

    DirectionalLight& getSun()
    {
        return m_sun;
    }
    const DirectionalLight& getSun() const
    {
        return m_sun;
    }

    void setAmbient(const glm::vec3& ambient)
    {
        m_ambient = ambient;
    }
    const glm::vec3& getAmbient() const
    {
        return m_ambient;
    }

private:
    std::vector<std::pair<Id, PointLight>> m_pointLights;
    DirectionalLight m_sun;
    glm::vec3 m_ambient = glm::vec3(0.45f);
    Id m_nextId = 1;
};

} // namespace spear::rendering

#endif
