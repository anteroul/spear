#include <spear/window.hh>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include <iostream>

namespace spear
{

Window::Window(const std::string& window_name, Size size, rendering::API api)
    : m_window(nullptr), m_size(size), m_api(api)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Couldn't initialize SDL3: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "SDL initialized" << std::endl;

    createWindow(window_name, size, api);
    initializeContext(api);
}

Window::~Window()
{
    switch (m_api)
    {
        case rendering::API::OpenGL:
            SDL_GL_DestroyContext(m_glContext);
            SDL_DestroyWindow(m_window);
            break;
        case rendering::API::Vulkan:
        case rendering::API::Metal:
        case rendering::API::DirectX12:
            perror("Unimplemented");
    }
    SDL_Quit();
}

void Window::createWindow(const std::string& title, Size size, rendering::API api)
{
    uint32_t flags;
    switch (api)
    {
        case rendering::API::OpenGL:
            flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
            SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "0"); // Disable OpenGL ES if on desktop
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            m_window = SDL_CreateWindow(title.c_str(), size.x, size.y, flags);
            if (!m_window)
            {
                throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
            }
            break;
        case rendering::API::Vulkan:
            // flags = SDL_WINDOW_VULKAN;
        case rendering::API::Metal:
        case rendering::API::DirectX12:
            perror("Unimplemented");
    }
}
void Window::initializeContext(rendering::API api)
{
    switch (api)
    {
        case rendering::API::OpenGL:
            m_glContext = SDL_GL_CreateContext(m_window);
            if (!m_glContext)
            {
                throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
            }
            if (!SDL_GL_MakeCurrent(m_window, m_glContext))
            {
                std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
                return;
            }
            // vsync
            // SDL_GL_SetSwapInterval(1);
            std::cout << "OpenGL context initialized.\n";
            break;
        case rendering::API::Vulkan:
        case rendering::API::Metal:
        case rendering::API::DirectX12:
            perror("Unimplemented");
    }
}

void Window::update(rendering::API api)
{
    switch (api)
    {
        case rendering::API::OpenGL:
            SDL_GL_SwapWindow(m_window);
            break;
        case rendering::API::Vulkan:
        case rendering::API::Metal:
        case rendering::API::DirectX12:
            perror("Unimplemented");
    }
}

void Window::hideCursor(bool hide)
{
    if (hide)
    {
        SDL_HideCursor();
    }
    else
    {
        SDL_ShowCursor();
    }
}

void Window::setMouseGrab(bool grabbed)
{
    if (!SDL_SetWindowMouseGrab(m_window, grabbed))
    {
        std::cerr << "Window: Could not set mouse grab" << std::endl;
    }
}

void Window::setRelativeMouseMode(bool is_relative)
{
    if (!SDL_SetWindowRelativeMouseMode(m_window, is_relative))
    {
        std::cerr << "Window: Could not set relative mouse mode!" << std::endl;
    }
}

void Window::resize()
{
    SDL_GetWindowSize(m_window, &m_size.x, &m_size.y);
}

} // namespace spear
