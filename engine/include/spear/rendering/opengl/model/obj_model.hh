#ifndef SPEAR_RENDERING_OPENGL_MODEL_OBJ_MODEL_HH
#define SPEAR_RENDERING_OPENGL_MODEL_OBJ_MODEL_HH

#include <spear/model/obj_loader.hh>
#include <spear/rendering/base_texture.hh>
#include <spear/rendering/opengl/model/model.hh>

namespace spear::rendering::opengl
{

class OBJModel : public Model
{
public:
    /// Constructor.
    OBJModel(const std::string& object_file_path, std::shared_ptr<BaseTexture> texture);

    /// Model::initialize implementation.
    void initialize() override;

    /// Mesh::render implementation.
    void render(Camera& camera) override;

private:
    OBJLoader m_loader;
    std::shared_ptr<BaseTexture> m_texture;

    // TODO this and other vao's should be stored in Mesh.
    uint32_t m_vao;

    std::vector<uint32_t> m_indices;
};

} // namespace spear::rendering::opengl

#endif
