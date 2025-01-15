#ifndef SPEAR_RENDERING_VULKAN_RENDERER_HH
#define SPEAR_RENDERING_VULKAN_RENDERER_HH

#include <spear/rendering/base_renderer.hh>
#include <spear/window/vulkan_window.hh>

namespace spear::rendering::vulkan
{

class Renderer : public BaseRenderer
{
public:
    /// Constructor.
    Renderer(VulkanWindow& vulkan_window);

    // Destructor.
    ~Renderer();

    void render() override;
    void setViewPort(int width, int height) override
    {
    }

    void setBackgroundColor(float r, float g, float b, float a) override
    {
    }
    void init() override;

private:
    void createDevice();
    void createSwapChain();
    void cleanup();

private:
    VkInstance& m_instance;
    VkSurfaceKHR& m_surface;
    VkDevice m_device = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
};

} // namespace spear::rendering::vulkan

#endif
