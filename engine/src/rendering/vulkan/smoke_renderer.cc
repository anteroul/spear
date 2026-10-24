#include <spear/rendering/vulkan/smoke_renderer.hh>

#include <glm/common.hpp>
#include <glm/mat4x4.hpp>

#include <array>
#include <cstring>
#include <stdexcept>

namespace spear::rendering::vulkan
{

namespace
{

constexpr uint32_t kWorkgroupSize = 64;
constexpr VkDeviceSize kParticleSize = sizeof(glm::vec4) * 4;

// Must match SmokeData in shaders/smoke_vulkan_compute.glsl (std140).
struct GpuSmokeEmitter
{
    glm::vec4 positionRadius;
    glm::vec4 velocityLifetime;
    glm::vec4 colorOpacity;
    glm::vec4 shape;
};

struct SmokeUniform
{
    glm::vec4 params;
    glm::vec4 wind;
    std::array<GpuSmokeEmitter, SmokeManager::maxEmitters> emitters;
};

// Must match the push constants in shaders/smoke_vulkan_vertex.glsl.
struct SmokePushConstants
{
    glm::mat4 viewProjection;
    glm::vec4 cameraRight;
    glm::vec4 cameraUp;
};

uint32_t memoryType(VkPhysicalDevice physical, uint32_t bits, VkMemoryPropertyFlags flags)
{
    VkPhysicalDeviceMemoryProperties properties{};
    vkGetPhysicalDeviceMemoryProperties(physical, &properties);
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
        if ((bits & (1u << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    throw std::runtime_error("No compatible smoke memory type");
}

void createBuffer(VkDevice device, VkPhysicalDevice physical, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags flags, VkBuffer& buffer, VkDeviceMemory& memory)
{
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &info, nullptr, &buffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create smoke buffer");
    VkMemoryRequirements requirements{};
    vkGetBufferMemoryRequirements(device, buffer, &requirements);
    VkMemoryAllocateInfo allocation{};
    allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = memoryType(physical, requirements.memoryTypeBits, flags);
    if (vkAllocateMemory(device, &allocation, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate smoke buffer");
    vkBindBufferMemory(device, buffer, memory, 0);
}

void bufferBarrier(VkCommandBuffer commandBuffer, VkBuffer buffer, VkPipelineStageFlags srcStage,
                   VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
{
    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = buffer;
    barrier.size = VK_WHOLE_SIZE;
    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 1, &barrier, 0, nullptr);
}

} // namespace

void SmokeRenderer::initialize(VkDevice device, VkPhysicalDevice physical)
{
    createBuffer(device, physical, kParticleSize * maxParticles,
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_particles, m_particleMemory);

    VkDescriptorSetLayoutBinding bindings[3]{};
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT;
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 3;
    info.pBindings = bindings;
    if (vkCreateDescriptorSetLayout(device, &info, nullptr, &m_layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create smoke descriptor layout");
}

void SmokeRenderer::cleanup(VkDevice device)
{
    cleanupFrameResources(device);
    if (m_layout)
        vkDestroyDescriptorSetLayout(device, m_layout, nullptr);
    if (m_particles)
        vkDestroyBuffer(device, m_particles, nullptr);
    if (m_particleMemory)
        vkFreeMemory(device, m_particleMemory, nullptr);
    m_layout = VK_NULL_HANDLE;
    m_particles = VK_NULL_HANDLE;
    m_particleMemory = VK_NULL_HANDLE;
}

void SmokeRenderer::createFrameResources(VkDevice device, VkPhysicalDevice physical,
                                         const std::vector<VkImageView>& depthViews)
{
    const auto count = static_cast<uint32_t>(depthViews.size());
    VkDescriptorPoolSize sizes[3] = {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, count},
                                     {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count},
                                     {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, count}};
    VkDescriptorPoolCreateInfo pool{};
    pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool.maxSets = count;
    pool.poolSizeCount = 3;
    pool.pPoolSizes = sizes;
    if (vkCreateDescriptorPool(device, &pool, nullptr, &m_pool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create smoke descriptor pool");

    m_frames.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        auto& frame = m_frames[i];
        createBuffer(device, physical, sizeof(SmokeUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, frame.uniform,
                     frame.uniformMemory);
        vkMapMemory(device, frame.uniformMemory, 0, sizeof(SmokeUniform), 0, &frame.mapped);

        VkDescriptorSetAllocateInfo setInfo{};
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setInfo.descriptorPool = m_pool;
        setInfo.descriptorSetCount = 1;
        setInfo.pSetLayouts = &m_layout;
        if (vkAllocateDescriptorSets(device, &setInfo, &frame.descriptor) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate smoke descriptor");

        VkDescriptorBufferInfo particleInfo{m_particles, 0, VK_WHOLE_SIZE};
        VkDescriptorBufferInfo uniformInfo{frame.uniform, 0, sizeof(SmokeUniform)};
        VkDescriptorImageInfo depthInfo{VK_NULL_HANDLE, depthViews[i], VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL};
        VkWriteDescriptorSet writes[3]{};
        for (uint32_t binding = 0; binding < 3; ++binding)
        {
            writes[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[binding].dstSet = frame.descriptor;
            writes[binding].dstBinding = binding;
            writes[binding].descriptorCount = 1;
        }
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writes[0].pBufferInfo = &particleInfo;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[1].pBufferInfo = &uniformInfo;
        writes[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        writes[2].pImageInfo = &depthInfo;
        vkUpdateDescriptorSets(device, 3, writes, 0, nullptr);
    }
}

void SmokeRenderer::cleanupFrameResources(VkDevice device)
{
    for (auto& frame : m_frames)
    {
        if (frame.uniform)
            vkDestroyBuffer(device, frame.uniform, nullptr);
        if (frame.uniformMemory)
            vkFreeMemory(device, frame.uniformMemory, nullptr);
    }
    m_frames.clear();
    if (m_pool)
        vkDestroyDescriptorPool(device, m_pool, nullptr);
    m_pool = VK_NULL_HANDLE;
}

void SmokeRenderer::recordSimulation(VkCommandBuffer commandBuffer, uint32_t imageIndex, const SmokeManager& smoke,
                                     float deltaTime, float time, VkPipeline pipeline, VkPipelineLayout layout)
{
    if (!smoke.getEmitters().empty())
        m_active = true;
    if (!m_active)
        return;

    SmokeUniform uniform{};
    uint32_t count = 0;
    for (const auto& [id, emitter] : smoke.getEmitters())
    {
        if (count == SmokeManager::maxEmitters)
            break;
        const float particles = emitter.enabled ? glm::clamp(emitter.rate * emitter.lifetime, 0.0f,
                                                             static_cast<float>(SmokeManager::maxParticlesPerEmitter))
                                                : 0.0f;
        auto& gpu = uniform.emitters[count++];
        gpu.positionRadius = glm::vec4(emitter.position, emitter.radius);
        gpu.velocityLifetime = glm::vec4(emitter.velocity, glm::max(emitter.lifetime, 0.1f));
        gpu.colorOpacity = glm::vec4(emitter.color, emitter.opacity);
        gpu.shape = glm::vec4(emitter.startSize, emitter.endSize, emitter.turbulence, particles);
    }
    uniform.params = glm::vec4(deltaTime, time, static_cast<float>(count), 0.0f);
    uniform.wind = glm::vec4(smoke.getWind(), 0.0f);
    std::memcpy(m_frames[imageIndex].mapped, &uniform, sizeof(uniform));

    if (!m_cleared)
    {
        // Zeroed particles are dead and get spawned by the first dispatch.
        vkCmdFillBuffer(commandBuffer, m_particles, 0, VK_WHOLE_SIZE, 0);
        bufferBarrier(commandBuffer, m_particles, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
        m_cleared = true;
    }
    else
    {
        // The previous frame's draw must finish reading before the particles are overwritten.
        bufferBarrier(commandBuffer, m_particles, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0,
                      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1,
                            &m_frames[imageIndex].descriptor, 0, nullptr);
    vkCmdDispatch(commandBuffer, (maxParticles + kWorkgroupSize - 1) / kWorkgroupSize, 1, 1);

    bufferBarrier(commandBuffer, m_particles, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_WRITE_BIT,
                  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);
}

void SmokeRenderer::recordDraw(VkCommandBuffer commandBuffer, uint32_t imageIndex, const Camera& camera,
                               VkExtent2D extent, VkPipeline pipeline, VkPipelineLayout layout,
                               VkDescriptorSet lightingSet)
{
    if (!m_active)
        return;

    const glm::mat4 view = camera.getViewMatrix();
    SmokePushConstants push{};
    push.viewProjection = camera.getProjectionMatrix() * view;
    push.cameraRight = glm::vec4(view[0][0], view[1][0], view[2][0], 0.0f);
    push.cameraUp = glm::vec4(view[0][1], view[1][1], view[2][1], 0.0f);

    VkViewport viewport{0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f};
    VkRect2D scissor{{0, 0}, extent};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDescriptorSet sets[] = {lightingSet, m_frames[imageIndex].descriptor};
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 2, sets, 0, nullptr);
    vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(push), &push);
    vkCmdDraw(commandBuffer, maxParticles * 6, 1, 0, 0);
}

} // namespace spear::rendering::vulkan
