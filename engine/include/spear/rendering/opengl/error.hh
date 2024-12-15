#ifndef SPEAR_RENDERING_OPENGL_ERROR_HH
#define SPEAR_RENDERING_OPENGL_ERROR_HH

#include <GL/glew.h>
#include <iostream>

namespace spear::rendering::opengl
{

static void openglError(const char* label)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error (" << label << "): " << error << std::endl;
    }
}

} // namespace spear::rendering::opengl

#endif
