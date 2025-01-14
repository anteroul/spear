#ifndef SPEAR_RENDERING_RENDERER_HH
#define SPEAR_RENDERING_RENDERER_HH

#include <spear/rendering/base_renderer.hh>
#include <spear/window/opengl_window.hh>

namespace spear::rendering::opengl
{

class Renderer : public BaseRenderer
{
public:
    /// Constructor.
    Renderer(OpenGLWindow& window);

    // Destructor.
    ~Renderer();

    void render() override;
    void setViewPort(int width, int height) override;
    void setBackgroundColor(float r, float g, float b, float a) override;
    void init() override;

private:
    SDL_GLContext m_context;
};

} // namespace spear::rendering::opengl

#endif
