#include <spear/rendering/opengl/renderer.hh>

#include <GL/glew.h>

#include <iostream>

namespace spear::rendering::opengl
{

Renderer::Renderer(OpenGLWindow& window)
    : BaseRenderer(window)
{
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
    m_context = SDL_GL_CreateContext(getWindow());
    if (!m_context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    }
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW initialization error: " << glewGetErrorString(err) << std::endl;
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Renderer::setViewPort(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setBackgroundColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

} // namespace spear::rendering::opengl
