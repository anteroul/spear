#include <spear/mesh.hh>

namespace spear
{

Mesh::Mesh(std::shared_ptr<rendering::BaseShader> shader)
    : m_shader(std::move(shader))
{
}

}
