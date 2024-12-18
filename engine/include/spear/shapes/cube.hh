#ifndef SPEAR_CUBE_HH
#define SPEAR_CUBE_HH

#include <spear/camera.hh>
#include <spear/shapes/shape.hh>

#include <spear/rendering/opengl/texture/sdl_texture.hh>
#include <spear/rendering/opengl/texture/stb_texture.hh>

#include <vector>

namespace spear
{

class Cube : public Shape
{
public:
    /// \param color The color of the sides of the cube.
    /// \param path The fixed path of the texture image.
    Cube(const glm::vec4& color, const std::string& path);

    // Mesh::render implementation.
    void render(Camera& camera) override;

private:
    std::vector<float> createVertexBufferData(const glm::vec3& v);
    std::vector<float> createUvData();
    void create(std::vector<float>&& vertexBufferData, std::vector<float>&& uvData);

private:
    // rendering::opengl::SDLTexture m_texture;
    rendering::opengl::STBTexture m_texture;
    uint32_t m_vao;
    glm::vec4 m_color;
    uint32_t m_vertexDataSize;
    uint32_t m_uvDataSize;
};

} // namespace spear

#endif
