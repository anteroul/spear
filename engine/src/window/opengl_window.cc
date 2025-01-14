#include <spear/window/opengl_window.hh>

namespace spear
{

OpenGLWindow::OpenGLWindow(const std::string& window_name, BaseWindow::Size size)
    : BaseWindow(window_name, size)
{
    createWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
}

void OpenGLWindow::update()
{
    SDL_GL_SwapWindow(BaseWindow::getSDLWindow());
}

} // namespace spear
