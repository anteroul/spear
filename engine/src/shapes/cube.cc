#include "spear/transform.hh"
#include <spear/rendering/opengl/error.hh>
#include <spear/shapes/cube.hh>

#include <SDL3/SDL.h>

#include <GL/glew.h>

namespace spear
{

Cube::Cube(const glm::vec4& color, const std::string& path)
    : Shape(std::shared_ptr<rendering::BaseShader>(rendering::opengl::Shader::create(rendering::ShaderType::cube))),
      m_texture(path),
      m_color(color)
{
    const glm::vec3 position = glm::vec3(1.0f, 1.0f, 1.0f);
    Transform::translate(position);
    create(createVertexBufferData(position), createUvData());
}

void Cube::create(std::vector<float>&& vertex_buffer_data, std::vector<float>&& uv_data)
{
    m_vertexDataSize = vertex_buffer_data.size();
    assert(vertex_buffer_data.size() == 108);
    assert(m_vertexDataSize == vertex_buffer_data.size());

    m_uvDataSize = uv_data.size();
    assert(uv_data.size() == 72);
    assert(m_uvDataSize == uv_data.size());

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    rendering::opengl::openglError("Cube::create glBindVertexArray");

    uint32_t vertexVBO;
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertexDataSize * sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    uint32_t uvVBO;
    glGenBuffers(1, &uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, m_uvDataSize * sizeof(float), uv_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    rendering::opengl::openglError("Cube::create VBO");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::render(Camera& camera)
{
    m_texture.bind();

    glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * Shape::Entity::Transform::getModel();
    Shape::Mesh::m_shader->use();

    m_shader->setMat4("mvp", mvp);
    m_shader->setSampler2D("textureSampler", 0);
    m_shader->setVec4f("color", m_color);

    glBindVertexArray(m_vao);
    // Draw constant.
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unset, unbind
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

std::vector<float> Cube::createVertexBufferData(const glm::vec3& v)
{
    // clang-format off
    return std::vector<float>
    {
        -v.x, -v.y, -v.z,
         v.x, -v.y, -v.z,
         v.x,  v.y, -v.z,
         v.x,  v.y, -v.z,
        -v.x,  v.y, -v.z,
        -v.x, -v.y, -v.z,

        -v.x, -v.y,  v.z,
         v.x, -v.y,  v.z,
         v.x,  v.y,  v.z,
         v.x,  v.y,  v.z,
        -v.x,  v.y,  v.z,
        -v.x, -v.y,  v.z,

        -v.x,  v.y,  v.z,
        -v.x,  v.y, -v.z,
        -v.x, -v.y, -v.z,
        -v.x, -v.y, -v.z,
        -v.x, -v.y,  v.z,
        -v.x,  v.y,  v.z,

         v.x,  v.y,  v.z,
         v.x,  v.y, -v.z,
         v.x, -v.y, -v.z,
         v.x, -v.y, -v.z,
         v.x, -v.y,  v.z,
         v.x,  v.y,  v.z,

        -v.x, -v.y, -v.z,
         v.x, -v.y, -v.z,
         v.x, -v.y,  v.z,
         v.x, -v.y,  v.z,
        -v.x, -v.y,  v.z,
        -v.x, -v.y, -v.z,

        -v.x,  v.y, -v.z,
         v.x,  v.y, -v.z,
         v.x,  v.y,  v.z,
         v.x,  v.y,  v.z,
        -v.x,  v.y,  v.z,
        -v.x,  v.y, -v.z
    };
    // clang-format on
}

std::vector<float> Cube::createUvData()
{
    // clang-format off
    return std::vector<float>
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    // clang-format on
}

} // namespace spear
