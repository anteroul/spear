#ifndef SPEAR_RENDERING_OPENGL_MODEL_GLTF_MODEL_HH
#define SPEAR_RENDERING_OPENGL_MODEL_GLTF_MODEL_HH

#include <spear/model/gltf_loader.hh>
#include <spear/rendering/base_model.hh>

#include <spear/physics/bullet/object_data.hh>

#include <cstdint>
#include <string>
#include <vector>

namespace spear::rendering::opengl
{

/// glTF 2.0 model (.gltf or .glb) drawn with the material shader, one draw per material.
class GLTFModel : public BaseModel
{
public:
    GLTFModel(const std::string& file_path, physics::bullet::ObjectData&& object_data);
    ~GLTFModel();

    GLTFModel(const GLTFModel&) = delete;
    GLTFModel& operator=(const GLTFModel&) = delete;

    void initialize() override
    {
    }

    void render(Camera& camera) override;

private:
    struct MaterialRenderData
    {
        uint32_t vao = 0;
        uint32_t vbo = 0;
        uint32_t texture = 0;
        int32_t vertexCount = 0;
    };

    void createMaterial(const GLTFLoader::Material& material);

    std::vector<MaterialRenderData> m_materialData;
};

} // namespace spear::rendering::opengl

#endif
