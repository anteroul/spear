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
    virtual ~VulkanWindow();

    void update() override;

    VkInstance createVulkanInstance();
    VkSurfaceKHR createVulkanSurface(VkInstance instance);
};

} // namespace spear

#endif
