#ifndef SPEAR_WINDOW_HH
#define SPEAR_WINDOW_HH

#include <spear/rendering/api.hh>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#include <cstdint>
#include <string>

namespace spear
{

class Window
{
public:
    struct Size
    {
        int x, y;
    };

    /// Constructor.
    Window(const std::string& window_name, Size size, rendering::API api);

    /// Destructor.
    ~Window();

    SDL_Window* getSDLWindow() const
    {
        return m_window;
    }

    void update(rendering::API api);
    void resize();
    Size getSize() const
    {
        return m_size;
    }

private:
    void createWindow(const std::string& title, Size size, rendering::API api);
    void initializeContext(rendering::API api);

private:
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    Size m_size;
    rendering::API m_api;
};

} // namespace spear

#endif
