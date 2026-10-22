#include <spear/rendering/lighting_uniform.hh>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace spear::rendering
{

namespace
{

glm::mat4 sunViewProjection(const DirectionalLight& sun, API api)
{
    const float e = sun.shadowExtent;
    const glm::mat4 projection = api == API::Vulkan ? glm::orthoRH_ZO(-e, e, -e, e, 1.0f, sun.shadowFar)
                                                    : glm::orthoRH_NO(-e, e, -e, e, 1.0f, sun.shadowFar);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    if (std::abs(glm::dot(glm::normalize(sun.direction), up)) > 0.999f)
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::mat4 view = glm::lookAt(sun.shadowCenter + glm::normalize(sun.direction) * sun.shadowDistance,
                                       sun.shadowCenter, up);
    return projection * view;
}

std::array<glm::mat4, 6> cubeFaces(const glm::vec3& position, float range, API api)
{
    glm::mat4 projection;
    if (api == API::Vulkan)
    {
        // Vulkan shaders negate gl_Position.y. Cancel that here so the cube faces land
        // in memory with the orientation cube map sampling expects, same as OpenGL.
        projection = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)) *
                     glm::perspectiveRH_ZO(glm::radians(90.0f), 1.0f, kPointShadowNear, range);
    }
    else
    {
        projection = glm::perspectiveRH_NO(glm::radians(90.0f), 1.0f, kPointShadowNear, range);
    }

    const glm::vec3& p = position;
    return {
            projection * glm::lookAt(p, p + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
            projection * glm::lookAt(p, p + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
            projection * glm::lookAt(p, p + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            projection * glm::lookAt(p, p + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
            projection * glm::lookAt(p, p + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
            projection * glm::lookAt(p, p + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)),
    };
}

} // namespace

LightingFrame buildLightingFrame(const LightManager& lights, const Camera* camera, glm::vec2 viewport, API api)
{
    LightingFrame frame;
    auto& u = frame.uniform;

    if (camera)
    {
        u.inverseViewProjection = glm::inverse(camera->getProjectionMatrix() * camera->getViewMatrix());
        u.cameraPosition = glm::vec4(camera->getPosition(), 1.0f);
    }
    u.viewport = glm::vec4(viewport, 0.0f, 0.0f);
    u.ambient = glm::vec4(lights.getAmbient(), 0.0f);

    const auto& sun = lights.getSun();
    frame.sunShadow = sun.castsShadow && sun.intensity > 0.0f;
    u.sunViewProjection = sunViewProjection(sun, api);
    u.sunDirection = glm::vec4(glm::normalize(sun.direction), frame.sunShadow ? 1.0f : 0.0f);
    u.sunColor = glm::vec4(sun.color, sun.intensity);
    u.shadowParams.x = kPointShadowNear;

    int count = 0;
    int shadowSlot = 0;
    for (const auto& [id, light] : lights.getPointLights())
    {
        if (!light.enabled || light.range <= kPointShadowNear)
            continue;
        if (count == static_cast<int>(LightManager::maxPointLights))
            break;

        auto& gpu = u.pointLights[count++];
        gpu.positionRange = glm::vec4(light.position, light.range);
        gpu.colorIntensity = glm::vec4(light.color, light.intensity);
        gpu.shadow = glm::ivec4(-1);

        if (light.castsShadow && shadowSlot < static_cast<int>(LightManager::maxShadowedPointLights))
        {
            gpu.shadow.x = shadowSlot;
            frame.pointShadows.push_back({shadowSlot, cubeFaces(light.position, light.range, api)});
            ++shadowSlot;
        }
    }
    u.counts.x = count;

    return frame;
}

} // namespace spear::rendering
