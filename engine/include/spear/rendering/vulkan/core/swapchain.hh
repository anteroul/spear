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
    void setExtent(int width, int height)
    {
        m_extent.width = width;
        m_extent.height = height;
    }

    void recreate(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const VulkanWindow& window);
    void createImageViews(VkDevice device);

private:
    bool isSwapchainSupported(VkPhysicalDevice physicalDevice);
    VkSurfaceCapabilitiesKHR querySurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    VkSurfaceFormatKHR chooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkPresentModeKHR choosePresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkSwapchainCreateInfoKHR createSwapchainCreateInfo(VkSurfaceKHR surface, const VkSurfaceCapabilitiesKHR& capabilities,
                                                       VkExtent2D extent, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode);
    void retrieveSwapchainImages(VkDevice device);

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat m_imageFormat;
    VkExtent2D m_extent;
};

} // namespace spear::rendering::vulkan

#endif
