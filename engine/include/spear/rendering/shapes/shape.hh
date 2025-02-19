#ifndef SPEAR_SHAPE_SHAPE_HH
#define SPEAR_SHAPE_SHAPE_HH

#include <spear/game_object.hh>
#include <spear/mesh.hh>

namespace spear::rendering
{

class Shape : public GameObject
{
public:
    Shape(std::shared_ptr<rendering::BaseShader> shader, physics::bullet::ObjectData&& object_data, const glm::vec4& color);

protected:
    glm::vec4 m_color;
};

} // namespace spear::rendering

#endif
