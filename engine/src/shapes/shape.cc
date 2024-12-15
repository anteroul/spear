#include <spear/shapes/shape.hh>

namespace spear
{

Shape::Shape(std::shared_ptr<rendering::BaseShader> shader)
    : Mesh(std::move(shader))
{
}

} // namespace spear
