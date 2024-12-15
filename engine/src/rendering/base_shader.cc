#include <spear/rendering/base_shader.hh>

#include <GL/glew.h>
#include <iostream>

namespace spear::rendering
{

BaseShader::BaseShader(BaseShader&& other)
    : m_id(std::move(other.m_id))
{
}

BaseShader& BaseShader::operator=(BaseShader&& other)
{
    if (this != &other)
    {
        m_id = std::move(other.m_id);
    }
    return *this;
}

void BaseShader::checkCompileErrors(uint32_t shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }
}

BaseShader::~BaseShader()
{
    // TODO
}

} // namespace spear::rendering
