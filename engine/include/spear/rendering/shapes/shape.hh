#ifndef SPEAR_SHAPE_SHAPE_HH
#define SPEAR_SHAPE_SHAPE_HH

#include <spear/entity.hh>
#include <spear/mesh.hh>

namespace spear::rendering
{

class Shape : public Mesh, public Entity
{
public:
    Shape(std::shared_ptr<rendering::BaseShader> shader, const glm::vec4& color);

protected:
    glm::vec4 m_color;
};

} // namespace spear::rendering

#endif
