#ifndef SPEAR_RENDERING_LIGHTING_UNIFORM_HH
#define SPEAR_RENDERING_LIGHTING_UNIFORM_HH

#include <spear/camera.hh>
#include <spear/rendering/api.hh>
#include <spear/rendering/light.hh>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <vector>

namespace spear::rendering
{

/// Near plane of the point light shadow cube maps.
constexpr float kPointShadowNear = 0.1f;

/// std140 layout, must match LightingData in shaders/lighting_common.glsl.
struct GpuPointLight
{
    glm::vec4 positionRange;  // xyz position, w range
    glm::vec4 colorIntensity; // rgb color, a intensity
    glm::ivec4 shadow;        // x shadow cube slot, -1 when not shadowed
};

/// std140 layout, must match LightingData in shaders/lighting_common.glsl.
struct LightingUniform
{
    glm::mat4 inverseViewProjection{1.0f};
    glm::mat4 sunViewProjection{1.0f};
    glm::vec4 sunDirection{0.0f};   // xyz towards the sun, w 1 when the sun casts shadows
    glm::vec4 sunColor{0.0f};       // rgb color, a intensity
    glm::vec4 ambient{0.0f};        // rgb
    glm::vec4 cameraPosition{0.0f}; // xyz
    glm::vec4 viewport{0.0f};       // xy framebuffer size
    glm::ivec4 counts{0};           // x point light count
    glm::vec4 shadowParams{0.0f};   // x point shadow near plane, y sun shadow texel size
    std::array<GpuPointLight, LightManager::maxPointLights> pointLights{};
};

/// Point light assigned to a shadow cube map slot this frame.
struct PointShadowView
{
    int slot = 0;
    /// View-projection per cube face, in +X, -X, +Y, -Y, +Z, -Z order.
    std::array<glm::mat4, 6> faces;
};

struct LightingFrame
{
    LightingUniform uniform;
    bool sunShadow = false;
    std::vector<PointShadowView> pointShadows;
};

/// Builds the per-frame lighting data. Shadow matrices follow the clip space conventions of the given API.
LightingFrame buildLightingFrame(const LightManager& lights, const Camera* camera, glm::vec2 viewport, API api);

} // namespace spear::rendering

#endif
