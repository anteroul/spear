#include <spear/rendering/shapes/base_cube.hh>

namespace spear
{

BaseCube::BaseCube(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture)
    : Shape(shader),
      m_texture(texture)
{
}

}
