#include <spear/rendering/base_model.hh>

#include <GL/glew.h>

namespace spear::rendering
{

BaseModel::BaseModel(std::shared_ptr<rendering::BaseShader> shader)
    : Mesh(std::move(shader))
{
}

} // namespace spear::rendering
