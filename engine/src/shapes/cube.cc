#include "spear/transform.hh"
#include <spear/rendering/opengl/error.hh>
#include <spear/shapes/cube.hh>

#include <SDL3/SDL.h>

#include <GL/glew.h>

namespace spear
{

Cube::Cube(rendering::opengl::Shader& shader, rendering::opengl::Texture& texture, const glm::vec3& color)
    : Shape(shader, texture, color), m_color(color)
{
    const glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
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

    Shape::Mesh::m_material.use();
    rendering::opengl::openglError("Cube::create Material use");
    glValidateProgram(Shape::Mesh::m_material.getProgram());
    GLint status;
    glGetProgramiv(Shape::Mesh::m_material.getProgram(), GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        std::cerr << "Shader program validation failed!" << std::endl;
    }

    // Uniform setup for the shader
    GLint textureLocation = glGetUniformLocation(Shape::Mesh::m_material.getProgram(), "textureSampler");
    if (textureLocation >= 0)
    {
        // Texture unit 0.
        glUniform1i(textureLocation, 0);
    }
    else
    {
        std::cerr << "Invalid textureSampler uniform location" << std::endl;
    }

    GLint colorLocation = Shape::Mesh::m_material.getColor();
    if (colorLocation >= 0)
    {
        glUniform4f(colorLocation, m_color.x, m_color.y, m_color.z, 1.0f);
    }
    else
    {
        std::cerr << "Invalid color uniform location" << std::endl;
    }

    rendering::opengl::openglError("Cube::create Uniform setup");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    if (current_program != 3)
    {
        std::cerr << "Shader program not active: " << current_program << std::endl;
    }
    if (Shape::Mesh::m_material.getMvp() < 0)
    {
        std::cerr << "Invalid mvp uniform location" << std::endl;
    }
    if (Shape::Mesh::m_material.getColor() < 0)
    {
        std::cerr << "Invalid color uniform location" << std::endl;
    }
}

void Cube::render(Camera& camera)
{
    Transform::rotate(1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    Transform::scale(glm::vec3(m_vertexDataSize * 1.0f));

    if (glIsProgram(Shape::Mesh::m_material.getProgram()) == GL_FALSE)
    {
        std::cerr << "Invalid shader program ID: " << Shape::Mesh::m_material.getProgram() << std::endl;
    }
    if (!glIsVertexArray(m_vao))
    {
        std::cerr << "Invalid VAO: " << m_vao << std::endl;
    }
    if (Shape::Mesh::m_material.getMvp() == -1)
    {
        std::cerr << "Uniform 'mvp' not found in shader program." << std::endl;
    }
    if (Shape::Mesh::m_material.getColor() == -1)
    {
        std::cerr << "Uniform 'color' not found in shader program." << std::endl;
    }
    if (!glIsTexture(Shape::Mesh::m_material.getTexture().getId()))
    {
        std::cerr << "Invalid texture ID: " << Shape::Mesh::m_material.getTexture().getId() << std::endl;
    }

    glm::mat4 mvp = camera.getProjectionMatrix(16.f / 9.f) * camera.getViewMatrix() * Shape::Entity::Transform::getModel();

    Shape::Mesh::m_material.use();
    if (glIsProgram(Shape::Mesh::m_material.getProgram()) == GL_FALSE)
    {
        std::cerr << "Invalid shader program ID: " << Shape::Mesh::m_material.getProgram() << std::endl;
        return;
    }

    // Bind texture.
    glActiveTexture(GL_TEXTURE0);
    if (!glIsTexture(Shape::Mesh::m_material.getTexture().getId()))
    {
        std::cerr << "Invalid texture ID: " << Shape::Mesh::m_material.getTexture().getId() << std::endl;
        return;
    }
    Shape::Mesh::m_material.getTexture().bind();

    // Bind VAO.
    if (!glIsVertexArray(m_vao))
    {
        std::cerr << "Invalid VAO: " << m_vao << std::endl;
        return;
    }
    glBindVertexArray(m_vao);

    // Set mvp.
    if (Shape::Mesh::m_material.getMvp() == -1)
    {
        std::cerr << "Uniform 'mvp' not found in shader program." << std::endl;
    }
    else
    {
        glUniformMatrix4fv(Shape::Mesh::m_material.getMvp(), 1, GL_FALSE, &mvp[0][0]);
    }

    // Set color.
    if (Shape::Mesh::m_material.getColor() == -1)
    {
        std::cerr << "Uniform 'color' not found or optimized out." << std::endl;
    }
    else
    {
        glUniform4f(Shape::Mesh::m_material.getColor(), m_color.x, m_color.y, m_color.z, 1.0f);
    }

    // Draw constant.
    glDrawArrays(GL_TRIANGLES, 0, 36);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error during glDrawArrays: " << error << std::endl;
    }

    // Unset, unbind
    glBindVertexArray(0);
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
