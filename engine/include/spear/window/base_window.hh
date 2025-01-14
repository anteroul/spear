#ifndef SPEAR_WINDOW_HH
#define SPEAR_WINDOW_HH

#include <SDL3/SDL.h>

#include <string>

namespace spear
{

class BaseWindow
{
public:
    struct Size
    {
        int x, y;
    };

    /// Constructor.
    BaseWindow(const std::string& window_name, Size size);

    /// Destructor.
    virtual ~BaseWindow();

    virtual void update() = 0;
    virtual void initializeContext() = 0;

    void resize();
    Size getSize() const
    {
        return m_size;
    }

    SDL_Window* getSDLWindow() const
    {
        return m_window;
    }

    /// Set visibility of the cursor.
    /// True for hidden cursor, false for visible cursor (default).
    void hideCursor(bool hide);

    /// Grab the mouse on to the screen.
    void setMouseGrab(bool grabbed);

    void setRelativeMouseMode(bool is_relative);

protected:
    void createWindow(uint32_t flags);

private:
    SDL_Window* m_window;
    std::string m_window_name;
    Size m_size;
};

} // namespace spear

#endif
