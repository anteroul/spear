#ifndef SPEAR_RENDERING_OPENGL_MODEL_MODEL_HH
#define SPEAR_RENDERING_OPENGL_MODEL_MODEL_HH

#include <spear/entity.hh>
#include <spear/mesh.hh>
#include <spear/rendering/base_shader.hh>

namespace spear::rendering::opengl
{

class Model : public Mesh, public Entity
{
public:
    /// Constructor.
    Model(std::shared_ptr<rendering::BaseShader> shader);

    virtual void initialize() = 0;
};

} // namespace spear::rendering::opengl

#endif
