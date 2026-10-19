#include <spear/rendering/vulkan/renderer.hh>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <stdexcept>

namespace spear::rendering::vulkan
{
namespace
{
constexpr uint32_t kShadowSize = 2048;

uint32_t memoryType(VkPhysicalDevice physical, uint32_t bits, VkMemoryPropertyFlags flags)
{
    VkPhysicalDeviceMemoryProperties properties{};
    vkGetPhysicalDeviceMemoryProperties(physical, &properties);
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    throw std::runtime_error("No compatible sun resource memory type");
}
}

glm::vec3 Renderer::getSunDirection() const
{
    // One slow daylight orbit every twenty minutes of game time.
    const float phase = m_sunTime * (6.283185307f / 1200.0f);
    const float elevation = 0.58f + 0.25f * std::sin(phase);
    const float azimuth = phase + 0.7f;
    return glm::normalize(glm::vec3(std::cos(azimuth) * std::cos(elevation),
                                    std::sin(elevation),
                                    std::sin(azimuth) * std::cos(elevation)));
}

void Renderer::createSunLayout()
{
    VkDescriptorSetLayoutBinding bindings[2]{};
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 2;
    info.pBindings = bindings;
    if (vkCreateDescriptorSetLayout(m_deviceManager.getDevice(), &info, nullptr, &m_sunLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create sun descriptor layout");

    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.magFilter = VK_FILTER_NEAREST;
    sampler.minFilter = VK_FILTER_NEAREST;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler.compareEnable = VK_TRUE;
    sampler.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    sampler.maxLod = 1.0f;
    if (vkCreateSampler(m_deviceManager.getDevice(), &sampler, nullptr, &m_shadowSampler) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shadow sampler");
}

void Renderer::createShadowRenderPass()
{
    VkAttachmentDescription depth{};
    depth.format = m_depthFormat;
    depth.samples = VK_SAMPLE_COUNT_1_BIT;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    VkAttachmentReference reference{0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pDepthStencilAttachment = &reference;
    VkSubpassDependency dependencies[2]{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &depth;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 2;
    info.pDependencies = dependencies;
    if (vkCreateRenderPass(m_deviceManager.getDevice(), &info, nullptr, &m_shadowRenderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shadow render pass");
}

void Renderer::createSunResources()
{
    VkDevice device = m_deviceManager.getDevice();
    VkPhysicalDevice physical = m_deviceManager.getPhysicalDevice();
    const uint32_t count = m_swapchain.getImageCount();
    VkDescriptorPoolSize sizes[2] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count}};
    VkDescriptorPoolCreateInfo pool{};
    pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool.maxSets = count;
    pool.poolSizeCount = 2;
    pool.pPoolSizes = sizes;
    if (vkCreateDescriptorPool(device, &pool, nullptr, &m_sunPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create sun descriptor pool");
    m_sunFrames.resize(count);
    for (auto& frame : m_sunFrames)
    {
        VkImageCreateInfo image{};
        image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image.imageType = VK_IMAGE_TYPE_2D;
        image.format = m_depthFormat;
        image.extent = {kShadowSize, kShadowSize, 1};
        image.mipLevels = 1;
        image.arrayLayers = 1;
        image.samples = VK_SAMPLE_COUNT_1_BIT;
        image.tiling = VK_IMAGE_TILING_OPTIMAL;
        image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        if (vkCreateImage(device, &image, nullptr, &frame.image) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shadow image");
        VkMemoryRequirements requirements{};
        vkGetImageMemoryRequirements(device, frame.image, &requirements);
        VkMemoryAllocateInfo allocation{};
        allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocation.allocationSize = requirements.size;
        allocation.memoryTypeIndex = memoryType(physical, requirements.memoryTypeBits,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkAllocateMemory(device, &allocation, nullptr, &frame.memory) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate shadow image");
        vkBindImageMemory(device, frame.image, frame.memory, 0);
        VkImageViewCreateInfo view{};
        view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view.image = frame.image;
        view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view.format = m_depthFormat;
        view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view.subresourceRange.levelCount = 1;
        view.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &view, nullptr, &frame.view) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shadow image view");
        VkFramebufferCreateInfo framebuffer{};
        framebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer.renderPass = m_shadowRenderPass;
        framebuffer.attachmentCount = 1;
        framebuffer.pAttachments = &frame.view;
        framebuffer.width = kShadowSize;
        framebuffer.height = kShadowSize;
        framebuffer.layers = 1;
        if (vkCreateFramebuffer(device, &framebuffer, nullptr, &frame.framebuffer) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shadow framebuffer");
        VkBufferCreateInfo buffer{};
        buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer.size = sizeof(SunUniform);
        buffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device, &buffer, nullptr, &frame.uniform) != VK_SUCCESS)
            throw std::runtime_error("Failed to create sun uniform buffer");
        vkGetBufferMemoryRequirements(device, frame.uniform, &requirements);
        allocation.allocationSize = requirements.size;
        allocation.memoryTypeIndex = memoryType(physical, requirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(device, &allocation, nullptr, &frame.uniformMemory) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate sun uniform buffer");
        vkBindBufferMemory(device, frame.uniform, frame.uniformMemory, 0);
        VkDescriptorSetAllocateInfo setInfo{};
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setInfo.descriptorPool = m_sunPool;
        setInfo.descriptorSetCount = 1;
        setInfo.pSetLayouts = &m_sunLayout;
        if (vkAllocateDescriptorSets(device, &setInfo, &frame.descriptor) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate sun descriptor");
        VkDescriptorBufferInfo bufferInfo{frame.uniform, 0, sizeof(SunUniform)};
        VkDescriptorImageInfo imageInfo{m_shadowSampler, frame.view,
                                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        VkWriteDescriptorSet writes[2]{};
        for (auto& write : writes)
        {
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = frame.descriptor;
            write.descriptorCount = 1;
        }
        writes[0].dstBinding = 0;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &bufferInfo;
        writes[1].dstBinding = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = &imageInfo;
        vkUpdateDescriptorSets(device, 2, writes, 0, nullptr);
    }
}

void Renderer::cleanupSunResources()
{
    VkDevice device = m_deviceManager.getDevice();
    for (auto& frame : m_sunFrames)
    {
        if (frame.framebuffer) vkDestroyFramebuffer(device, frame.framebuffer, nullptr);
        if (frame.view) vkDestroyImageView(device, frame.view, nullptr);
        if (frame.image) vkDestroyImage(device, frame.image, nullptr);
        if (frame.memory) vkFreeMemory(device, frame.memory, nullptr);
        if (frame.uniform) vkDestroyBuffer(device, frame.uniform, nullptr);
        if (frame.uniformMemory) vkFreeMemory(device, frame.uniformMemory, nullptr);
    }
    m_sunFrames.clear();
    if (m_sunPool) vkDestroyDescriptorPool(device, m_sunPool, nullptr);
    m_sunPool = VK_NULL_HANDLE;
}
} // namespace spear::rendering::vulkan
