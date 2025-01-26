#include <spear/rendering/vulkan/texture/texture.hh>

#include <iostream>

namespace spear::rendering::vulkan
{

Texture::Texture()
    : BaseTexture(),
      m_device(VK_NULL_HANDLE),
      m_image(VK_NULL_HANDLE),
      m_imageView(VK_NULL_HANDLE),
      m_sampler(VK_NULL_HANDLE)
{
}

Texture::Texture(Texture&& other)
    : BaseTexture(std::move(other)),
      m_device(other.m_device),
      m_image(other.m_image),
      m_imageMemory(other.m_imageMemory),
      m_imageView(other.m_imageView),
      m_sampler(other.m_sampler)
{
    other.m_image = VK_NULL_HANDLE;
    other.m_imageView = VK_NULL_HANDLE;
    other.m_sampler = VK_NULL_HANDLE;
}

Texture& Texture::operator=(Texture&& other)
{
    if (this != &other)
    {
        BaseTexture::operator=(std::move(other));
        m_device = other.m_device;
        m_image = other.m_image;
        m_imageMemory = other.m_imageMemory;
        m_imageView = other.m_imageView;
        m_sampler = other.m_sampler;

        other.m_image = VK_NULL_HANDLE;
        other.m_imageView = VK_NULL_HANDLE;
        other.m_sampler = VK_NULL_HANDLE;
    }
    return *this;
}

Texture::~Texture()
{
    cleanup();
}

void Texture::cleanup()
{
    if (m_imageView)
        vkDestroyImageView(m_device, m_imageView, nullptr);
    if (m_image)
        vkDestroyImage(m_device, m_image, nullptr);
    if (m_imageMemory)
        vkFreeMemory(m_device, m_imageMemory, nullptr);
    if (m_sampler)
        vkDestroySampler(m_device, m_sampler, nullptr);
}

void Texture::bind(uint32_t unit) const
{
    std::cout << "Not allowed in Vulkan" << std::endl;
}

void Texture::unbind(uint32_t unit)
{
    std::cout << "Not allowed in Vulkan" << std::endl;
}

} // namespace spear::rendering::vulkan
