#ifndef SPEAR_RENDERING_OPENGL_BASE_MODEL_HH
#define SPEAR_RENDERING_OPENGL_BASE_MODEL_HH

#include <spear/entity.hh>
#include <spear/mesh.hh>
#include <spear/rendering/base_shader.hh>

namespace spear::rendering
{

class BaseModel : public Mesh, public Entity
{
public:
    /// Constructor.
    BaseModel(std::shared_ptr<rendering::BaseShader> shader);

    virtual void initialize() = 0;
};

} // namespace spear::rendering

#endif
