#ifndef SPEAR_WINDOW_OPENGL_WINDOW_HH
#define SPEAR_WINDOW_OPENGL_WINDOW_HH

#include <spear/window/base_window.hh>

namespace spear
{

class OpenGLWindow : public BaseWindow
{
public:
    /// Constructor.
    OpenGLWindow(const std::string& window_name, BaseWindow::Size size);

    /// Destructor.
    ~OpenGLWindow() = default;

    void update() override;
};

} // namespace spear

#endif
