#ifndef SPEAR_RENDERING_BASE_RENDERER_HH
#define SPEAR_RENDERING_BASE_RENDERER_HH

#include <spear/rendering/light.hh>
#include <spear/window/base_window.hh>

#include <glm/vec4.hpp>

#include <optional>

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

    /// Sun, ambient and point lights used for the next rendered frame.
    LightManager& getLights()
    {
        return m_lights;
    }
    const LightManager& getLights() const
    {
        return m_lights;
    }

protected:
    const BaseWindow& getWindow() const
    {
        return m_baseWindow;
    }

    void setBackgroundColor(const glm::vec4& background_color)
    {
        m_backgroundColor = background_color;
    }

    std::optional<glm::vec4> getBackgroundColor() const
    {
        return m_backgroundColor;
    }

    bool hasBackgroundColor() const
    {
        return m_backgroundColor.has_value();
    }

private:
    BaseWindow& m_baseWindow;
    std::optional<glm::vec4> m_backgroundColor;
    LightManager m_lights;
};

} // namespace spear::rendering

#endif
