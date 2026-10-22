#ifndef SPEAR_RENDERING_OPENGL_MODEL_OBJ_MODEL_HH
#define SPEAR_RENDERING_OPENGL_MODEL_OBJ_MODEL_HH

#include <spear/model/obj_loader.hh>
#include <spear/rendering/base_model.hh>
#include <spear/rendering/base_texture.hh>

#include <spear/physics/bullet/object_data.hh>

namespace spear::rendering::opengl
{

class OBJModel : public BaseModel
{
public:
    /// Constructor.
    OBJModel(const std::string& object_file_path, const std::string& material_file_path, std::shared_ptr<BaseTexture> texture, physics::bullet::ObjectData&& object_data);

    /// Model::initialize implementation.
    void initialize() override;

    const OBJLoader& getLoader() const
    {
        return m_loader;
    }

    /// Mesh::render implementation.
    void render(Camera& camera) override;

private:
    OBJLoader m_loader;
    std::shared_ptr<BaseTexture> m_texture;

    uint32_t m_vao;
    uint32_t m_vertexCount = 0;

    ModelLoader::MaterialEntry m_material;
};

} // namespace spear::rendering::opengl

#endif
