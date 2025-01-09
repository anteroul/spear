#ifndef SPEAR_RENDERING_OPENGL_SHAPES_SPHERE_HH
#define SPEAR_RENDERING_OPENGL_SHAPES_SPHERE_HH

#include <spear/physics/bullet/object_data.hh>
#include <spear/rendering/shapes/textured_shape.hh>

#include <vector>

namespace spear::rendering::opengl
{

class Sphere : public TexturedShape
{
public:
    /// Constructor.
    Sphere(std::shared_ptr<rendering::BaseTexture> texture, physics::bullet::ObjectData&& object_data, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
           int latitudeBands = 20, int longitudeBands = 20);

    /// Mesh::render implementation.
    void render(Camera& camera) override;

private:
    void create(std::vector<float>&& vertex_buffer_data, std::vector<float>&& uv_data, std::vector<uint32_t>&& indices);
    std::vector<float> createVertexBufferData(int latitudeBands, int longitudeBands, const glm::vec3& position);
    std::vector<float> createUvData(int latitudeBands, int longitudeBands);
    std::vector<uint32_t> createIndexBufferData(uint32_t segments, uint32_t rings);

private:
    uint32_t m_vao;
    size_t m_numVertices;
};

} // namespace spear::rendering::opengl

#endif
