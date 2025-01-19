#ifndef SPEAR_RENDERING_VULKAN_PIPELINE_MANAGER_HH
#define SPEAR_RENDERING_VULKAN_PIPELINE_MANAGER_HH

#include <vulkan/vulkan.h>

namespace spear::rendering::vulkan
{

class PipelineManager
{
public:
    void initialize(VkDevice device, VkRenderPass renderPass, VkExtent2D extent, VkPipelineLayout pipelineLayout);
    void cleanup(VkDevice device);

    VkPipeline getPipeline() const
    {
        return m_graphicsPipeline;
    }

private:
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

} // namespace spear::rendering::vulkan

#endif
