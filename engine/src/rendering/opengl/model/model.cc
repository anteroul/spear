#include <spear/rendering/opengl/model/model.hh>

#include <GL/glew.h>

namespace spear::rendering::opengl
{

Model::Model(std::shared_ptr<rendering::BaseShader> shader)
    : Mesh(std::move(shader))
{
}

} // namespace spear::rendering::opengl
