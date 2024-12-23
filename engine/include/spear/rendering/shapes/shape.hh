#ifndef SPEAR_SHAPE_SHAPE_HH
#define SPEAR_SHAPE_SHAPE_HH

#include <spear/entity.hh>
#include <spear/mesh.hh>

namespace spear
{

class Shape : public Mesh, public Entity
{
public:
    Shape(std::shared_ptr<rendering::BaseShader> shader);
};

} // namespace spear

#endif
