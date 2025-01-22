#include <spear/rendering/vulkan/renderer.hh>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <iostream>

namespace spear::rendering::vulkan
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
};

Renderer::Renderer(VulkanWindow& vulkan_window)
    : BaseRenderer(vulkan_window)
{
    auto window_size = vulkan_window.getSize();
    m_instance = vulkan_window.createVulkanInstance();
    m_surface = vulkan_window.createVulkanSurface(m_instance);

    m_deviceManager.initialize(m_instance, m_surface);
    m_swapchain.initialize(m_deviceManager.getDevice(), m_deviceManager.getPhysicalDevice(), m_surface, window_size.x, window_size.y);
    m_renderPassManager.initialize(m_deviceManager.getDevice(), m_swapchain.getFormat());
    m_frameBufferManager.initialize(m_deviceManager.getDevice(), m_renderPassManager.getRenderPass(), m_swapchain.getImageViews(), m_swapchain.getExtent());
    m_pipelineManager.initialize(m_deviceManager.getDevice(), m_renderPassManager.getRenderPass(), m_swapchain.getExtent());
    m_commandBufferManager.initialize(m_deviceManager.getDevice(), m_deviceManager.getCommandPool(), m_swapchain.getImageCount());
    m_synchronization.initialize(m_deviceManager.getDevice(), m_framesInFlight);
}

Renderer::~Renderer()
{
    cleanup();
}

void Renderer::init()
{
}

void Renderer::render()
{
    drawFrame();
}

void Renderer::drawFrame()
{
    VkDevice device = m_deviceManager.getDevice();
    if (device == VK_NULL_HANDLE)
    {
        std::cerr << "Device is null" << std::endl;
        return;
    }
    VkQueue graphicsQueue = m_deviceManager.getGraphicsQueue();
    VkQueue presentQueue = m_deviceManager.getPresentQueue();

    auto fence = m_synchronization.getInFlightFence(m_currentFrame);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence);

    // Acquire an image from the swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device,
                                            m_swapchain.getSwapchain(),
                                            UINT64_MAX,
                                            m_synchronization.getImageAvailableSemaphore(m_currentFrame),
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        try
        {
            recreateSwapchain();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Swapchain recreation failed: " << e.what() << std::endl;
        }
        std::cout << "Recreated swap chain" << std::endl;
        return;
    }
    else if (result == VK_SUBOPTIMAL_KHR)
    {
        std::cout << "VK_SUBOPTIMAL_KHR: Swapchain is suboptimal, but rendering will proceed." << std::endl;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    // Record command buffer
    m_commandBufferManager.beginCommandBuffer(imageIndex);
    VkCommandBuffer commandBuffer = m_commandBufferManager.getCommandBuffers()[imageIndex];
    if (commandBuffer == VK_NULL_HANDLE)
    {
        std::cerr << "Command buffer is null" << std::endl;
        return;
    }

    auto render_pass = m_renderPassManager.getRenderPass();
    if (render_pass == VK_NULL_HANDLE)
    {
        std::cerr << "Render pass is null" << std::endl;
        return;
    }
    auto frame_buffer = m_frameBufferManager.getFrameBuffers()[imageIndex];
    if (frame_buffer == VK_NULL_HANDLE)
    {
        std::cerr << "Frame buffer is null" << std::endl;
        return;
    }
    auto extent = m_swapchain.getExtent();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = render_pass;
    renderPassInfo.framebuffer = frame_buffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    std::array<VkClearValue, 2> clearValues{};
    if (hasBackgroundColor())
    {
        auto bg_color = BaseRenderer::getBackgroundColor().value();
        clearValues[0].color = {{bg_color.r, bg_color.g, bg_color.b, bg_color.a}};
    }
    else
    {
        clearValues[0].color = {{1.0f, 1.0f, 1.0f, 1.0f}};
    }
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    auto pipeline = m_pipelineManager.getPipeline();
    if (pipeline == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Pipeline is null");
    }

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    // vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    m_commandBufferManager.endCommandBuffer(commandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_synchronization.getImageAvailableSemaphore(m_currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {m_synchronization.getRenderFinishedSemaphore(m_currentFrame)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_synchronization.getInFlightFence(m_currentFrame)) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Present the image
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapchain.getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}

void Renderer::cleanSwapchain()
{
    vkDeviceWaitIdle(m_deviceManager.getDevice());
    m_frameBufferManager.cleanup(m_deviceManager.getDevice());
    m_commandBufferManager.cleanup();
    m_swapchain.cleanup(m_deviceManager.getDevice());
}

void Renderer::cleanup()
{
    cleanSwapchain();
    m_synchronization.cleanup(m_deviceManager.getDevice());
    m_pipelineManager.cleanup(m_deviceManager.getDevice());
    m_renderPassManager.cleanup(m_deviceManager.getDevice());
    m_deviceManager.cleanup();
}

void Renderer::setViewPort(int width, int height)
{
    m_swapchain.setExtent(width, height);

    // Update the viewport.
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Update the scissor rectangle.
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    // Ensure a valid command buffer recording session
    VkCommandBuffer commandBuffer = m_commandBufferManager.getCommandBuffers()[m_currentFrame];
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // Begin recording if not already in progress
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer: " << result << std::endl;
        return;
    }

    // Record viewport and scissor commands
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // End command buffer recording
    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to end command buffer: " << result << std::endl;
        return;
    }
}

void Renderer::setBackgroundColor(float r, float g, float b, float a)
{
    BaseRenderer::setBackgroundColor(glm::vec4(r, g, b, a));
}

void Renderer::recreateSwapchain()
{
    cleanSwapchain();
    const auto& vulkan_window = *dynamic_cast<const VulkanWindow*>(&BaseRenderer::getWindow());
    auto window_size = vulkan_window.getSize();
    std::cout << "New size x: " << window_size.x << " y: " << window_size.y << std::endl;

    m_swapchain.recreate(m_deviceManager.getPhysicalDevice(), m_deviceManager.getDevice(), m_surface, vulkan_window);
    m_swapchain.createImageViews(m_deviceManager.getDevice());
    setViewPort(window_size.x, window_size.y);
    m_frameBufferManager.initialize(m_deviceManager.getDevice(), m_renderPassManager.getRenderPass(), m_swapchain.getImageViews(), m_swapchain.getExtent());
    m_commandBufferManager.initialize(m_deviceManager.getDevice(), m_deviceManager.getCommandPool(), m_swapchain.getImageCount());
}

} // namespace spear::rendering::vulkan
