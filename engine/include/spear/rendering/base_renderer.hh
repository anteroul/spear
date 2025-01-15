#ifndef SPEAR_RENDERING_BASE_RENDERER_HH
#define SPEAR_RENDERING_BASE_RENDERER_HH

#include <spear/window/base_window.hh>

namespace spear::rendering
{

class BaseRenderer
{
public:
    /// Constructor.
    BaseRenderer(BaseWindow& window);

    /// Destructor.
    virtual ~BaseRenderer()
    {
    }

    virtual void init() = 0;
    virtual void render() = 0;
    virtual void setViewPort(int width, int height) = 0;
    virtual void setBackgroundColor(float r, float g, float b, float a) = 0;

protected:
    const BaseWindow& getWindow() const
    {
        return m_baseWindow;
    }

private:
    BaseWindow& m_baseWindow;
};

} // namespace spear::rendering

#endif
