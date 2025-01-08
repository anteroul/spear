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

    /// Mesh::render implementation.
    void render(Camera& camera) override;

    void setLightPosition(glm::vec3 light_position)
    {
        m_lightPosition = light_position;
    }

    void setLightColor(glm::vec3 light_color)
    {
        m_lightColor = light_color;
    }

    void setLightIntensity(float light_intensity)
    {
        m_lightIntensity = light_intensity;
    }

private:
    OBJLoader m_loader;
    std::shared_ptr<BaseTexture> m_texture;

    uint32_t m_vao;

    std::vector<uint32_t> m_indices;
    ModelLoader::Material m_material;

    glm::vec3 m_lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_lightIntensity = 1.5f;
};

} // namespace spear::rendering::opengl

#endif
