#include <spear/material.hh>
#include <spear/rendering/opengl/error.hh>
#include <spear/rendering/opengl/shader.hh>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <iostream>

namespace spear
{

Material::Material(rendering::opengl::Shader& shader, rendering::opengl::Texture& texture)
    : m_shader(shader),
      m_texture(texture),
      m_mvp(-1),
      m_color(-1),
      m_sampler(-1)
{
    // Ensure the shader program is created and linked.
    if (!m_shader.isProgramLinked())
    {
        m_shader.createShaderProgram();
    }

    // Query uniform locations
    m_mvp = glGetUniformLocation(m_shader.getId(), "mvp");
    if (m_mvp == -1)
    {
        std::cerr << "Error: Uniform 'mvp' not found in shader program." << std::endl;
    }

    m_color = glGetUniformLocation(m_shader.getId(), "color");
    if (m_color == -1)
    {
        std::cerr << "Error: Uniform 'color' not found in shader program." << std::endl;
    }

    m_sampler = glGetUniformLocation(m_shader.getId(), "textureSampler");
    if (m_sampler == -1)
    {
        std::cerr << "Error: Uniform 'textureSampler' not found in shader program." << std::endl;
    }
}

void Material::use()
{
    SDL_GLContext currentContext = SDL_GL_GetCurrentContext();
    if (!currentContext)
    {
        std::cerr << "No active OpenGL context! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    GLuint programId = m_shader.getId();
    if (!glIsProgram(programId))
    {
        std::cerr << "Invalid shader program ID: " << programId << std::endl;
        return;
    }

    GLint linkStatus = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        std::cerr << "Shader program linking failed!" << std::endl;
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Linker error log:\n"
                  << infoLog << std::endl;
        return;
    }

    GLint validateStatus = 0;
    glValidateProgram(programId);
    glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE)
    {
        std::cerr << "Shader program validation failed!" << std::endl;
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Validation error log:\n"
                  << infoLog << std::endl;
        return;
    }

    // Ensure no program pipeline is active
    GLint pipeline = 0;
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &pipeline);
    if (pipeline != 0)
    {
        std::cerr << "Program pipeline active. Cannot use glUseProgram!" << std::endl;
        return;
    }

    // Use the shader program
    glUseProgram(programId);
    rendering::opengl::openglError("glUseProgram");

    // Check if the texture ID is valid
    GLuint textureId = m_texture.getId();
    if (textureId == 0)
    {
        std::cerr << "Invalid texture ID: " << textureId << std::endl;
        return;
    }

    // Bind the texture after the program is active
    glActiveTexture(GL_TEXTURE0);
    rendering::opengl::openglError("glActiveTexture");

    glBindTexture(GL_TEXTURE_2D, textureId);
    rendering::opengl::openglError("glBindTexture");

    // Ensure the sampler location is valid
    if (m_sampler == -1)
    {
        std::cerr << "Invalid sampler uniform location!" << std::endl;
        return;
    }

    // Set the uniform for the texture
    glUniform1i(m_sampler, 0);
    rendering::opengl::openglError("glUniform1i");
}

} // namespace spear
