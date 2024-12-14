#ifndef SPEAR_SHAPE_SHAPE_HH
#define SPEAR_SHAPE_SHAPE_HH

#include <spear/mesh.hh>
#include <spear/entity.hh>

namespace spear
{

class Shape : public Mesh, public Entity
{
public:
    Shape(std::shared_ptr<rendering::BaseShader> shader);
};

}

#endif
