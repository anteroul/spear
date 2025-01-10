#include <spear/rendering/shapes/shape.hh>

namespace spear::rendering
{

Shape::Shape(std::shared_ptr<rendering::BaseShader> shader, physics::bullet::ObjectData&& object_data, const glm::vec4& color)
    : GameObject(std::move(object_data), shader),
      m_color(color)
{
}

} // namespace spear::rendering
