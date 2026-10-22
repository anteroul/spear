#include <spear/rendering/vulkan/frame_context.hh>
#include <spear/rendering/vulkan/renderer.hh>
#include <spear/scene.hh>

#include <cstring>
#include <stdexcept>

namespace spear::rendering::vulkan
{
namespace
{
constexpr uint32_t kShadowSize = 2048;
constexpr uint32_t kPointShadowSize = 512;
constexpr uint32_t kPointShadowCount = LightManager::maxShadowedPointLights;

uint32_t memoryType(VkPhysicalDevice physical, uint32_t bits, VkMemoryPropertyFlags flags)
{
    VkPhysicalDeviceMemoryProperties properties{};
    vkGetPhysicalDeviceMemoryProperties(physical, &properties);
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    throw std::runtime_error("No compatible sun resource memory type");
}

void createDepthImage(VkDevice device, VkPhysicalDevice physical, VkFormat format, uint32_t size,
                      uint32_t layers, VkImage& image, VkDeviceMemory& memory)
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.flags = layers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = {size, size, 1};
    info.mipLevels = 1;
    info.arrayLayers = layers;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    if (vkCreateImage(device, &info, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shadow image");
    VkMemoryRequirements requirements{};
    vkGetImageMemoryRequirements(device, image, &requirements);
    VkMemoryAllocateInfo allocation{};
    allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = memoryType(physical, requirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (vkAllocateMemory(device, &allocation, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate shadow image");
    vkBindImageMemory(device, image, memory, 0);
}

VkImageView createDepthView(VkDevice device, VkImage image, VkFormat format, VkImageViewType type,
                            uint32_t baseLayer, uint32_t layers)
{
    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = image;
    view.viewType = type;
    view.format = format;
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view.subresourceRange.levelCount = 1;
    view.subresourceRange.baseArrayLayer = baseLayer;
    view.subresourceRange.layerCount = layers;
    VkImageView result = VK_NULL_HANDLE;
    if (vkCreateImageView(device, &view, nullptr, &result) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shadow image view");
    return result;
}

VkFramebuffer createShadowFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView view, uint32_t size)
{
    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = renderPass;
    info.attachmentCount = 1;
    info.pAttachments = &view;
    info.width = size;
    info.height = size;
    info.layers = 1;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    if (vkCreateFramebuffer(device, &info, nullptr, &framebuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shadow framebuffer");
    return framebuffer;
}
} // namespace

void Renderer::createSunLayout()
{
    VkDescriptorSetLayoutBinding bindings[3]{};
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = kPointShadowCount;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 3;
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
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count * (1 + kPointShadowCount)}};
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
        createDepthImage(device, physical, m_depthFormat, kShadowSize, 1, frame.image, frame.memory);
        frame.view = createDepthView(device, frame.image, m_depthFormat, VK_IMAGE_VIEW_TYPE_2D, 0, 1);
        frame.framebuffer = createShadowFramebuffer(device, m_shadowRenderPass, frame.view, kShadowSize);

        for (auto& point : frame.pointShadows)
        {
            createDepthImage(device, physical, m_depthFormat, kPointShadowSize, 6, point.image, point.memory);
            point.cubeView = createDepthView(device, point.image, m_depthFormat, VK_IMAGE_VIEW_TYPE_CUBE, 0, 6);
            for (uint32_t face = 0; face < 6; ++face)
            {
                point.faceViews[face] = createDepthView(device, point.image, m_depthFormat,
                                                        VK_IMAGE_VIEW_TYPE_2D, face, 1);
                point.framebuffers[face] = createShadowFramebuffer(device, m_shadowRenderPass,
                                                                   point.faceViews[face], kPointShadowSize);
            }
        }

        VkBufferCreateInfo buffer{};
        buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer.size = sizeof(LightingUniform);
        buffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device, &buffer, nullptr, &frame.uniform) != VK_SUCCESS)
            throw std::runtime_error("Failed to create sun uniform buffer");
        VkMemoryRequirements requirements{};
        vkGetBufferMemoryRequirements(device, frame.uniform, &requirements);
        VkMemoryAllocateInfo allocation{};
        allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
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
        VkDescriptorBufferInfo bufferInfo{frame.uniform, 0, sizeof(LightingUniform)};
        VkDescriptorImageInfo imageInfo{m_shadowSampler, frame.view,
                                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        std::array<VkDescriptorImageInfo, kPointShadowCount> cubeInfos{};
        for (uint32_t i = 0; i < kPointShadowCount; ++i)
            cubeInfos[i] = {m_shadowSampler, frame.pointShadows[i].cubeView,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        VkWriteDescriptorSet writes[3]{};
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
        writes[2].dstBinding = 2;
        writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[2].descriptorCount = kPointShadowCount;
        writes[2].pImageInfo = cubeInfos.data();
        vkUpdateDescriptorSets(device, 3, writes, 0, nullptr);
    }
    transitionShadowImages();
}

void Renderer::transitionShadowImages()
{
    // Shadow maps that are not rendered in a frame are still bound for sampling, so every
    // image starts out in the layout the descriptors declare.
    VkDevice device = m_deviceManager.getDevice();
    VkCommandBufferAllocateInfo allocation{};
    allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocation.commandPool = m_deviceManager.getCommandPool();
    allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocation.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(device, &allocation, &commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate shadow transition command buffer");

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &begin);

    std::vector<VkImageMemoryBarrier> barriers;
    auto addBarrier = [&barriers](VkImage image, uint32_t layers)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = layers;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barriers.push_back(barrier);
    };
    for (const auto& frame : m_sunFrames)
    {
        addBarrier(frame.image, 1);
        for (const auto& point : frame.pointShadows)
            addBarrier(point.image, 6);
    }
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0, 0, nullptr, 0, nullptr, static_cast<uint32_t>(barriers.size()), barriers.data());
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(m_deviceManager.getGraphicsQueue(), 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_deviceManager.getGraphicsQueue());
    vkFreeCommandBuffers(device, m_deviceManager.getCommandPool(), 1, &commandBuffer);
}

void Renderer::recordShadowPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, uint32_t size,
                                const glm::mat4& lightViewProjection)
{
    VkClearValue clear{};
    clear.depthStencil = {1.0f, 0};
    VkRenderPassBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin.renderPass = m_shadowRenderPass;
    begin.framebuffer = framebuffer;
    begin.renderArea.extent = {size, size};
    begin.clearValueCount = 1;
    begin.pClearValues = &clear;
    vkCmdBeginRenderPass(commandBuffer, &begin, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size), 0.0f, 1.0f};
    VkRect2D scissor{{0, 0}, {size, size}};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    g_frameContext.commandBuffer = commandBuffer;
    g_frameContext.shadowPass = true;
    g_frameContext.lightViewProjection = lightViewProjection;
    g_frameContext.shadowColorPipeline = m_pipelineManager.getShadowColorPipeline();
    g_frameContext.shadowTexturedPipeline = m_pipelineManager.getShadowTexturedPipeline();
    g_frameContext.shadowPipelineLayout = m_pipelineManager.getShadowPipelineLayout();
    m_scene->update(*m_camera);
    g_frameContext = {};
    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::cleanupSunResources()
{
    VkDevice device = m_deviceManager.getDevice();
    for (auto& frame : m_sunFrames)
    {
        for (auto& point : frame.pointShadows)
        {
            for (auto framebuffer : point.framebuffers)
                if (framebuffer)
                    vkDestroyFramebuffer(device, framebuffer, nullptr);
            for (auto view : point.faceViews)
                if (view)
                    vkDestroyImageView(device, view, nullptr);
            if (point.cubeView)
                vkDestroyImageView(device, point.cubeView, nullptr);
            if (point.image)
                vkDestroyImage(device, point.image, nullptr);
            if (point.memory)
                vkFreeMemory(device, point.memory, nullptr);
        }
        if (frame.framebuffer)
            vkDestroyFramebuffer(device, frame.framebuffer, nullptr);
        if (frame.view)
            vkDestroyImageView(device, frame.view, nullptr);
        if (frame.image)
            vkDestroyImage(device, frame.image, nullptr);
        if (frame.memory)
            vkFreeMemory(device, frame.memory, nullptr);
        if (frame.uniform)
            vkDestroyBuffer(device, frame.uniform, nullptr);
        if (frame.uniformMemory)
            vkFreeMemory(device, frame.uniformMemory, nullptr);
    }
    m_sunFrames.clear();
    if (m_sunPool)
        vkDestroyDescriptorPool(device, m_sunPool, nullptr);
    m_sunPool = VK_NULL_HANDLE;
}

// Called from drawFrame: uploads this frame's lighting and records every shadow pass.
void Renderer::recordLighting(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    const auto extent = m_swapchain.getExtent();
    LightingFrame lighting = buildLightingFrame(getLights(), m_camera,
                                                glm::vec2(extent.width, extent.height), API::Vulkan);
    lighting.uniform.shadowParams.y = 1.0f / static_cast<float>(kShadowSize);

    auto& frame = m_sunFrames[imageIndex];
    void* data = nullptr;
    vkMapMemory(m_deviceManager.getDevice(), frame.uniformMemory, 0, sizeof(LightingUniform), 0, &data);
    std::memcpy(data, &lighting.uniform, sizeof(LightingUniform));
    vkUnmapMemory(m_deviceManager.getDevice(), frame.uniformMemory);

    if (!m_camera || !m_scene)
        return;

    if (lighting.sunShadow)
        recordShadowPass(commandBuffer, frame.framebuffer, kShadowSize, lighting.uniform.sunViewProjection);
    for (const auto& point : lighting.pointShadows)
        for (uint32_t face = 0; face < 6; ++face)
            recordShadowPass(commandBuffer, frame.pointShadows[point.slot].framebuffers[face],
                             kPointShadowSize, point.faces[face]);
}

} // namespace spear::rendering::vulkan
