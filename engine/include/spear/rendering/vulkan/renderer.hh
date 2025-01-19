#ifndef SPEAR_RENDERING_VULKAN_RENDERER_HH
#define SPEAR_RENDERING_VULKAN_RENDERER_HH

#include <spear/rendering/base_renderer.hh>
#include <spear/window/vulkan_window.hh>

#include <spear/rendering/vulkan/core/command_buffer_manager.hh>
#include <spear/rendering/vulkan/core/device_manager.hh>
#include <spear/rendering/vulkan/core/framebuffer_manager.hh>
#include <spear/rendering/vulkan/core/pipeline_manager.hh>
#include <spear/rendering/vulkan/core/render_pass_manager.hh>
#include <spear/rendering/vulkan/core/swapchain.hh>
#include <spear/rendering/vulkan/core/synchronization.hh>

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

    void drawFrame();

private:
    void cleanup();
    VkInstance createInstance();
    VkSurfaceKHR createSurface();

private:
    DeviceManager m_deviceManager;
    Swapchain m_swapchain;
    CommandBufferManager m_commandBufferManager;
    FramebufferManager m_frameBufferManager;
    PipelineManager m_pipelineManager;
    RenderPassManager m_renderPassManager;
    Synchronization m_synchronization;

    uint64_t m_currentFrame = 0;
    uint64_t m_framesInFlight = 2;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
};

} // namespace spear::rendering::vulkan

#endif
