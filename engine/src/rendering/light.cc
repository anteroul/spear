#include <spear/rendering/light.hh>

#include <glm/geometric.hpp>

#include <algorithm>
#include <cmath>

namespace spear::rendering
{

glm::vec3 sunDirectionAt(float seconds)
{
    // One slow daylight orbit every twenty minutes of game time.
    const float phase = seconds * (6.283185307f / 1200.0f);
    const float elevation = 0.58f + 0.25f * std::sin(phase);
    const float azimuth = phase + 0.7f;
    return glm::normalize(glm::vec3(std::cos(azimuth) * std::cos(elevation),
                                    std::sin(elevation),
                                    std::sin(azimuth) * std::cos(elevation)));
}

LightManager::Id LightManager::addPointLight(const PointLight& light)
{
    const Id id = m_nextId++;
    m_pointLights.emplace_back(id, light);
    return id;
}

bool LightManager::removePointLight(Id id)
{
    auto it = std::find_if(m_pointLights.begin(), m_pointLights.end(), [id](const auto& entry)
                           { return entry.first == id; });
    if (it == m_pointLights.end())
        return false;
    m_pointLights.erase(it);
    return true;
}

void LightManager::clearPointLights()
{
    m_pointLights.clear();
}

PointLight* LightManager::getPointLight(Id id)
{
    for (auto& [lightId, light] : m_pointLights)
    {
        if (lightId == id)
            return &light;
    }
    return nullptr;
}

} // namespace spear::rendering
