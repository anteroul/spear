#ifndef SPEAR_RENDERING_SHAPES_BASE_CUBE_HH
#define SPEAR_RENDERING_SHAPES_BASE_CUBE_HH

#include <spear/rendering/shapes/shape.hh>

namespace spear
{

class BaseCube : public Shape
{
public:
    /// Constructor.
    BaseCube(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture);
protected:
    std::shared_ptr<rendering::BaseTexture> m_texture;
};

}

#endif
