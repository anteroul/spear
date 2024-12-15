#ifndef SPEAR_SPRITE_3D_HH
#define SPEAR_SPRITE_3D_HH

#include <spear/rendering/base_texture.hh>
#include <spear/rendering/opengl/error.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/texture.hh>

#include <spear/mesh.hh>
#include <spear/transform.hh>

namespace spear
{

class Sprite3D : public Mesh, public Transform
{
public:
    // Constructor with texture.
    Sprite3D(glm::vec3 position);

    // Destructor.
    ~Sprite3D();

    void initialize();
    void loadImage(const std::string& path);

    /// Mesh::render implementation.
    void render(Camera& camera) override;

    void setPosition(const glm::vec3& newPosition)
    {
        m_position = newPosition;
    }

    void setRotation(float newRotation)
    {
        m_rotation = newRotation;
    }

private:
    rendering::opengl::Texture m_texture;

    // Input data.
    glm::vec3 m_position;
    float m_rotation; // degrees

    // Vertex Array Object, Vertex Buffer Object, Element Buffer Object.
    uint32_t m_vao = 0, m_vbo = 0, m_ebo = 0;

    // Vertices for a quad.
    /*
    const float m_vertices[20] = {
        // Positions        // Texture Coords
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f
    };
    */

    // clang-format off
    const float m_vertices[20] = {
        // Positions        // Texture Coords
       -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // Top-left
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top-right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom-right
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // Bottom-left
    };

    const uint32_t m_indices[6] = {
        0, 1, 2,
        2, 3, 0
    };
    // clang-format on

    int32_t m_sampler;
};

} // namespace spear

#endif
