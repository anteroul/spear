#ifndef SPEAR_RENDERING_VULKAN_SMOKE_RENDERER_HH
#define SPEAR_RENDERING_VULKAN_SMOKE_RENDERER_HH

#include <spear/camera.hh>
#include <spear/rendering/smoke.hh>

#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace spear::rendering::vulkan
{

/// GPU smoke particles: a compute shader simulates them, then they are drawn as soft billboards
/// in the main render pass' second subpass, which reads the scene depth as an input attachment.
class SmokeRenderer
{
public:
    static constexpr uint32_t maxParticles =
            static_cast<uint32_t>(SmokeManager::maxEmitters * SmokeManager::maxParticlesPerEmitter);

    /// Creates the particle buffer and descriptor layout. Call once.
    void initialize(VkDevice device, VkPhysicalDevice physical);
    void cleanup(VkDevice device);

    /// Per swapchain image emitter buffers and descriptor sets. Recreated with the swapchain.
    void createFrameResources(VkDevice device, VkPhysicalDevice physical, const std::vector<VkImageView>& depthViews);
    void cleanupFrameResources(VkDevice device);

    /// Set 0 of the compute pipeline and set 1 of the draw pipeline.
    VkDescriptorSetLayout getLayout() const
    {
        return m_layout;
    }

    /// Records the simulation step. Must be outside a render pass.
    void recordSimulation(VkCommandBuffer commandBuffer, uint32_t imageIndex, const SmokeManager& smoke,
                          float deltaTime, float time, VkPipeline pipeline, VkPipelineLayout layout);

    /// Records the particle draw. Must be inside the main render pass' smoke subpass.
    void recordDraw(VkCommandBuffer commandBuffer, uint32_t imageIndex, const Camera& camera, VkExtent2D extent,
                    VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet lightingSet);

    /// Particles are simulated and drawn once any emitter has been added.
    bool isActive() const
    {
        return m_active;
    }

private:
    struct Frame
    {
        VkBuffer uniform = VK_NULL_HANDLE;
        VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
        void* mapped = nullptr;
        VkDescriptorSet descriptor = VK_NULL_HANDLE;
    };

    VkBuffer m_particles = VK_NULL_HANDLE;
    VkDeviceMemory m_particleMemory = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    std::vector<Frame> m_frames;
    bool m_cleared = false;
    bool m_active = false;
};

} // namespace spear::rendering::vulkan

#endif
