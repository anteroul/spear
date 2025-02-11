#ifndef SPEAR_RENDERING_VULKAN_SYNCHRONIZATION_HH
#define SPEAR_RENDERING_VULKAN_SYNCHRONIZATION_HH

#include <vulkan/vulkan.h>

#include <vector>

namespace spear::rendering::vulkan
{

class Synchronization
{
public:
    void initialize(VkDevice device, size_t maxFramesInFlight);
    void cleanup(VkDevice device);

    VkSemaphore getImageAvailableSemaphore(size_t index) const
    {
        return m_imageAvailableSemaphores[index];
    }
    VkSemaphore getRenderFinishedSemaphore(size_t index) const
    {
        return m_renderFinishedSemaphores[index];
    }
    VkFence& getInFlightFence(size_t index)
    {
        return m_inFlightFences[index];
    }

private:
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};

} // namespace spear::rendering::vulkan

#endif
