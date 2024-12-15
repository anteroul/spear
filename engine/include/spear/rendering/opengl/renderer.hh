#ifndef SPEAR_RENDERING_RENDERER_HH
#define SPEAR_RENDERING_RENDERER_HH

#include <GL/glew.h>
#include <SDL3/SDL.h>

namespace spear::rendering::opengl
{

class Renderer
{
public:
    /// Constructor.
    Renderer(SDL_Window* window);

    // Destructor.
    ~Renderer();

    void render();
    void setViewPort(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void setBackgroundColor(float r, float g, float b, float a);
    void init();

private:
private:
    SDL_Window* m_window;
    uint32_t m_shader_program;
    SDL_GLContext m_context;
};

} // namespace spear::rendering::opengl

#endif
