#ifndef SPEAR_RENDERING_OPENGL_SHAPES_CUBE_HH
#define SPEAR_RENDERING_OPENGL_SHAPES_CUBE_HH

#include <spear/camera.hh>
#include <spear/rendering/base_texture.hh>
#include <spear/rendering/shapes/base_cube.hh>

#include <vector>

namespace spear::rendering::opengl
{

class Cube : public BaseCube
{
public:
    /// Constructor.
    /// \param texture The texture used in the cube.
    /// \param color The color of the sides of the cube.
    Cube(std::shared_ptr<rendering::BaseTexture> texture, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Mesh::render implementation.
    void render(Camera& camera) override;

private:
    std::vector<float> createVertexBufferData(const glm::vec3& v);
    std::vector<float> createUvData();
    void create(std::vector<float>&& vertexBufferData, std::vector<float>&& uvData);

private:
    uint32_t m_vao;
    glm::vec4 m_color;
};

} // namespace spear::rendering::opengl

#endif
