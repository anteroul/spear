#ifndef SPEAR_RENDERING_VULKAN_TEXTURE_TEXTURE_HH
#define SPEAR_RENDERING_VULKAN_TEXTURE_TEXTURE_HH

#include <spear/rendering/base_texture.hh>

#include <vulkan/vulkan.h>

namespace spear::rendering::vulkan
{

class Texture : public BaseTexture
{
public:
    /// Default constructor.
    Texture();

    /// Destructor.
    ~Texture() override;

    /// Move constructor.
    Texture(Texture&& other);

    /// Move assignment operator.
    Texture& operator=(Texture&& other);

    /// Deleted copy constructor.
    Texture(const Texture& other) = delete;

    /// Deleted copy assignment operator.
    Texture& operator=(const Texture& other) = delete;

    void bind(uint32_t unit = 0) const override;
    void unbind(uint32_t unit = 0) override;

protected:
    void cleanup();

protected:
    VkDevice m_device;
    VkImage m_image;
    VkDeviceMemory m_imageMemory;
    VkImageView m_imageView;
    VkSampler m_sampler;
};

} // namespace spear::rendering::vulkan

#endif
