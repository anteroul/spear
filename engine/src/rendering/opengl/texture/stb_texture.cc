#include <spear/rendering/opengl/texture/stb_texture.hh>
#include <spear/spear_root.hh>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../third_party/stb_image.h"

#include <GL/glew.h>

#include <iostream>

namespace spear::rendering::opengl
{

STBTexture::STBTexture(const std::string& path)
{
    std::string used_path = spearRoot() + "/assets/" + path;

    // Generate texture object
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data from file using stb_image
    int width, height, numComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(used_path.c_str(), &width, &height, &numComponents, 0);

    if (data)
    {
        // Upload image data to texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cerr << "Failed to create texture: " << used_path << std::endl;
    }

    // Free image data
    stbi_image_free(data);

    std::cout << "Created texture: " << used_path << " using stb" << std::endl;
}

} // namespace spear::rendering::opengl
