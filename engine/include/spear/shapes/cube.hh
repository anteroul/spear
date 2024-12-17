#ifndef SPEAR_CUBE_HH
#define SPEAR_CUBE_HH

#include "spear/rendering/opengl/texture.hh"
#include <spear/camera.hh>
#include <spear/shapes/shape.hh>
#include <spear/stb_texture.hh>

#include <vector>

namespace spear
{

class Cube : public Shape
{
public:
    Cube(const glm::vec4& color, const std::string& path);

    // Mesh::render implementation.
    void render(Camera& camera) override;

private:
    std::vector<float> createVertexBufferData(const glm::vec3& v);
    std::vector<float> createUvData();
    void create(std::vector<float>&& vertexBufferData, std::vector<float>&& uvData);

private:
    // StbTexture m_texture;
    rendering::opengl::Texture m_texture;
    uint32_t m_vao;
    glm::vec4 m_color;
    uint32_t m_vertexDataSize;
    uint32_t m_uvDataSize;
};

} // namespace spear

#endif
