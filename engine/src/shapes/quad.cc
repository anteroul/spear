#include <spear/shapes/quad.hh>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace spear
{

Quad::Quad(const glm::vec3& color)
    : Shape(std::shared_ptr<rendering::BaseShader>(rendering::opengl::Shader::create(rendering::ShaderType::quad))),
      m_color(color),
      m_vao(0), m_vbo(0), m_ebo(0)
{
}

Quad::~Quad()
{
    free();
}

void Quad::free()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Quad::translate(const glm::vec3& position)
{
    free();
    initialize(position);
}

void Quad::initialize(const glm::vec3& position)
{
    m_vertices =
    {
        position.x-0.5f, position.y-0.5f, position.z-0.5f,  m_color.x, m_color.y, m_color.z,
        position.x+0.5f, position.y-0.5f, position.z-0.5f,  m_color.x, m_color.y, m_color.z,
        position.x+0.5f, position.y+0.5f, position.z-0.5f,  m_color.x, m_color.y, m_color.z,
        position.x-0.5f, position.y+0.5f, position.z-0.5f,  m_color.x, m_color.y, m_color.z,
        position.x-0.5f, position.y-0.5f, position.z+0.5f,  m_color.x, m_color.y, m_color.z,
        position.x+0.5f, position.y-0.5f, position.z+0.5f,  m_color.x, m_color.y, m_color.z,
        position.x+0.5f, position.y+0.5f, position.z+0.5f,  m_color.x, m_color.y, m_color.z,
        position.x-0.5f, position.y+0.5f, position.z+0.5f,  m_color.x, m_color.y, m_color.z,
    };

    uint32_t indices[]
    {
        0, 1, 2, 2, 3, 0,   // Front face
        4, 5, 6, 6, 7, 4,   // Back face
        0, 1, 5, 5, 4, 0,   // Bottom face
        3, 2, 6, 6, 7, 3,   // Top face
        0, 3, 7, 7, 4, 0,   // Left face
        1, 2, 6, 6, 5, 1    // Right face
    };

    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Positions
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Colors
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Quad::render(Camera& camera)
{
    m_shader->use();
    glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * glm::mat4(1);
    Mesh::m_shader->setMat4("mvp", mvp);

    // Render the cube
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
