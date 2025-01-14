#ifndef SPEAR_WINDOW_VULKAN_WINDOW
#define SPEAR_WINDOW_VULKAN_WINDOW

#include <spear/window/base_window.hh>

#include <vulkan/vulkan.h>

namespace spear
{

class VulkanWindow : public BaseWindow
{
public:
    /// Constructor.
    VulkanWindow(const std::string& window_name, BaseWindow::Size size);

    /// Destructor.
    ~VulkanWindow();

    void initializeContext() override;
    void update() override;

private:
    void createVulkanInstance();
    void createVulkanSurface();

private:
    VkInstance m_vkInstance;
    VkSurfaceKHR m_vkSurface;
};

} // namespace spear

#endif
