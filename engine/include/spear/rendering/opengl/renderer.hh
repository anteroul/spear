#ifndef SPEAR_RENDERING_RENDERER_HH
#define SPEAR_RENDERING_RENDERER_HH

#include <spear/camera.hh>
#include <spear/rendering/base_renderer.hh>
#include <spear/rendering/lighting_uniform.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/window/opengl_window.hh>

#include <array>
#include <memory>

namespace spear
{
class Scene;
}

namespace spear::rendering::opengl
{

class Renderer : public BaseRenderer
{
public:
    /// Constructor.
    Renderer(OpenGLWindow& window);

    // Destructor.
    ~Renderer();

    /// Renders shadow maps, uploads lighting and, when a scene and camera are set, draws the scene.
    void render() override;
    void setViewPort(int width, int height) override;
    void setBackgroundColor(float r, float g, float b, float a) override;
    void init() override;

    void setCamera(Camera* camera)
    {
        m_camera = camera;
    }
    void setScene(Scene* scene)
    {
        m_scene = scene;
    }

private:
    void createShadowResources();
    void renderShadowPass(uint32_t size, const glm::mat4& lightViewProjection);

private:
    SDL_GLContext m_context;

    Camera* m_camera = nullptr;
    Scene* m_scene = nullptr;
    int m_viewportWidth = 0;
    int m_viewportHeight = 0;

    std::shared_ptr<Shader> m_shadowShader;
    uint32_t m_lightingBuffer = 0;
    uint32_t m_shadowFramebuffer = 0;
    uint32_t m_sunShadowMap = 0;
    std::array<uint32_t, LightManager::maxShadowedPointLights> m_pointShadowMaps{};
};

} // namespace spear::rendering::opengl

#endif
