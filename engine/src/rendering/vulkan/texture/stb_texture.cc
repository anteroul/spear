#include <spear/rendering/vulkan/texture/stb_texture.hh>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../third_party/stb_image.h"

#include <iostream>

namespace spear::rendering::vulkan
{

STBTexture::STBTexture(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
    : Texture(),
      m_physicalDevice(physicalDevice), m_commandPool(commandPool), m_graphicsQueue(graphicsQueue)
{
    m_device = device;
}

void STBTexture::loadFromFile(const std::string& filePath)
{
    // Load image using stb_image
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels)
    {
        throw std::runtime_error("Failed to load texture image!");
    }

    setWidth(texWidth);
    setHeight(texHeight);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    // Create a Vulkan staging buffer and transfer the image to the GPU
    // This involves creating a VkBuffer, copying data, creating a VkImage, and transitioning layouts
    createTextureImage(pixels, imageSize);

    stbi_image_free(pixels);
}

void STBTexture::createTextureImage(const void* pixelData, VkDeviceSize imageSize)
{
    // Create and upload to staging buffer, create Vulkan image, and transfer data
    // Placeholder implementation for brevity
    std::cout << "Creating Vulkan texture image...\n";
}

} // namespace spear::rendering::vulkan
