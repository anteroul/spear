#ifndef SPEAR_RENDERING_SHAPES_TEXTURED_SHAPE_HH
#define SPEAR_RENDERING_SHAPES_TEXTURED_SHAPE_HH

#include <spear/rendering/shapes/shape.hh>

namespace spear::rendering
{

class TexturedShape : public Shape
{
public:
    /// Constructor.
    TexturedShape(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture, physics::bullet::ObjectData&& object_data, const glm::vec4& color);

protected:
    std::shared_ptr<rendering::BaseTexture> m_texture;
};

} // namespace spear::rendering

#endif
