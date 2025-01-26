#ifndef SPEAR_RENDERING_VULKAN_TEXTURE_STB_TEXTURE_HH
#define SPEAR_RENDERING_VULKAN_TEXTURE_STB_TEXTURE_HH

#include <spear/rendering/vulkan/texture/texture.hh>

namespace spear::rendering::vulkan
{

class STBTexture : public Texture
{
public:
    /// Constructor.
    STBTexture(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);

    void loadFromFile(const std::string& filePath);

private:
    void createTextureImage(const void* pixelData, VkDeviceSize imageSize);

private:
    VkPhysicalDevice m_physicalDevice;
    VkCommandPool m_commandPool;
    VkQueue m_graphicsQueue;
};

} // namespace spear::rendering::vulkan

#endif
