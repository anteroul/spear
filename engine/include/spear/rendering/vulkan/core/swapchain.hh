#ifndef SPEAR_RENDERING_VULKAN_SWAPCHAIN_HH
#define SPEAR_RENDERING_VULKAN_SWAPCHAIN_HH

#include <spear/window/vulkan_window.hh>

#include <vector>

namespace spear::rendering::vulkan
{

class Swapchain
{
public:
    void initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int width, int height);
    void cleanup(VkDevice device);

    VkSwapchainKHR getSwapchain() const
    {
        return m_swapchain;
    }
    const std::vector<VkImageView>& getImageViews() const
    {
        return m_imageViews;
    }
    uint32_t getImageCount() const
    {
        return static_cast<uint32_t>(m_images.size());
    }
    VkFormat getFormat() const
    {
        return m_imageFormat;
    }
    VkExtent2D getExtent() const
    {
        return m_extent;
    }
    void recreate(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const VulkanWindow& window);

private:
    void createImageViews(VkDevice device);

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat m_imageFormat;
    VkExtent2D m_extent;
};

} // namespace spear::rendering::vulkan

#endif
