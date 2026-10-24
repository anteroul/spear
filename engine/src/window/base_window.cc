#include <SDL3/SDL_video.h>
#include <spear/window/base_window.hh>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <iostream>

namespace spear
{

BaseWindow::BaseWindow(const std::string& window_name, Size size)
    : m_window(nullptr),
      m_window_name(window_name),
      m_size(size)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Couldn't initialize SDL3: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "SDL initialized" << std::endl;
}

BaseWindow::~BaseWindow()
{
    if (m_cursorLockedToCenter)
        SDL_RemoveEventWatch(cursorLockEventWatch, this);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void BaseWindow::createWindow(uint32_t flags)
{
    m_window = SDL_CreateWindow(m_window_name.c_str(), m_size.x, m_size.y, flags);
    if (!m_window)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
    if (!SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED))
    {
        std::cerr << "Window: Could not center window: " << SDL_GetError() << std::endl;
    }
    // The window manager applies the position asynchronously; wait for it so that anything
    // placed in screen coordinates afterwards (such as the cursor lock) lands in the right spot.
    SDL_SyncWindow(m_window);
}

void BaseWindow::hideCursor(bool hide)
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

void BaseWindow::setMouseGrab(bool grabbed)
{
    if (!SDL_SetWindowMouseGrab(m_window, grabbed))
    {
        std::cerr << "Window: Could not set mouse grab" << std::endl;
    }
}

void BaseWindow::setRelativeMouseMode(bool is_relative)
{
    if (!SDL_SetWindowRelativeMouseMode(m_window, is_relative))
    {
        std::cerr << "Window: Could not set relative mouse mode!" << std::endl;
    }
}

void BaseWindow::lockCursorToCenter(bool locked)
{
    if (locked && !m_cursorLockedToCenter)
        SDL_AddEventWatch(cursorLockEventWatch, this);
    else if (!locked && m_cursorLockedToCenter)
        SDL_RemoveEventWatch(cursorLockEventWatch, this);
    m_cursorLockedToCenter = locked;
    if (locked)
    {
        // The grab keeps the cursor inside the window between recentering warps.
        setMouseGrab(true);
        recenterCursor();
    }
}

bool SDLCALL BaseWindow::cursorLockEventWatch(void* userdata, SDL_Event* event)
{
    auto* window = static_cast<BaseWindow*>(userdata);
    const SDL_WindowID id = SDL_GetWindowID(window->m_window);
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        if (event->motion.windowID == id)
            window->recenterCursor();
        break;
    case SDL_EVENT_WINDOW_MOVED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        if (event->window.windowID == id)
            window->recenterCursor();
        break;
    default:
        break;
    }
    return true;
}

void BaseWindow::recenterCursor()
{
    if (!(SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS))
        return;
    // Query the geometry here, since the event watch can run before resize() updates m_size.
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    SDL_GetWindowPosition(m_window, &x, &y);
    SDL_GetWindowSize(m_window, &width, &height);
    // Warp globally: a warp within the window is ignored in relative mouse mode. Relative
    // motion comes from the device, so the warp does not turn into mouse motion itself.
    SDL_WarpMouseGlobal(static_cast<float>(x + width / 2), static_cast<float>(y + height / 2));
}

bool BaseWindow::setIcon(const std::string& image_path)
{
    SDL_Surface* icon = IMG_Load(image_path.c_str());
    if (!icon)
    {
        std::cerr << "Window: Could not load icon " << image_path << ": " << SDL_GetError() << std::endl;
        return false;
    }
    const bool result = SDL_SetWindowIcon(m_window, icon);
    if (!result)
    {
        std::cerr << "Window: Could not set icon: " << SDL_GetError() << std::endl;
    }
    SDL_DestroySurface(icon);
    return result;
}

void BaseWindow::resize()
{
    SDL_GetWindowSize(m_window, &m_size.x, &m_size.y);
}

} // namespace spear
