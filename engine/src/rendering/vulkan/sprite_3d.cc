#include <spear/rendering/vulkan/frame_context.hh>
#include <spear/rendering/vulkan/sprite_3d.hh>

#include <glm/mat4x4.hpp>

#include <cstring>
#include <stdexcept>

namespace spear::rendering::vulkan
{

static uint32_t findMemoryType(VkPhysicalDevice physDevice,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physDevice, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Sprite3D: failed to find suitable memory type!");
}

Sprite3D::Sprite3D(VkDevice device,
                   VkPhysicalDevice physDevice,
                   std::shared_ptr<vulkan::Texture> texture,
                   VkDescriptorPool descriptorPool,
                   VkDescriptorSetLayout descriptorSetLayout,
                   physics::bullet::ObjectData&& object_data)
    : BaseSprite3D(texture, nullptr, std::move(object_data)),
      m_vkTexture(std::move(texture)),
      m_device(device),
      m_descriptorPool(descriptorPool)
{
    createVertexBuffer(physDevice);

    VkDescriptorSetAllocateInfo dsAllocInfo{};
    dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsAllocInfo.descriptorPool = descriptorPool;
    dsAllocInfo.descriptorSetCount = 1;
    dsAllocInfo.pSetLayouts = &descriptorSetLayout;
    if (vkAllocateDescriptorSets(m_device, &dsAllocInfo, &m_descriptorSet) != VK_SUCCESS)
        throw std::runtime_error("Sprite3D: failed to allocate descriptor set!");

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_vkTexture->getImageView();
    imageInfo.sampler = m_vkTexture->getSampler();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}

Sprite3D::~Sprite3D()
{
    if (m_descriptorSet != VK_NULL_HANDLE)
        vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_descriptorSet);
    if (m_vertexBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
    if (m_vertexMemory != VK_NULL_HANDLE)
        vkFreeMemory(m_device, m_vertexMemory, nullptr);
}

void Sprite3D::render(Camera& camera)
{
    VkCommandBuffer cmd = g_frameContext.commandBuffer;
    VkPipeline pipeline = g_frameContext.shadowPass ? g_frameContext.shadowTexturedPipeline : g_frameContext.texturedPipeline;
    VkPipelineLayout layout = g_frameContext.shadowPass ? g_frameContext.shadowPipelineLayout : g_frameContext.texturedPipelineLayout;
    if (cmd == VK_NULL_HANDLE || pipeline == VK_NULL_HANDLE || layout == VK_NULL_HANDLE)
        return;

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    if (!g_frameContext.shadowPass)
    {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_descriptorSet, 0, nullptr);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1,
                                &g_frameContext.sunDescriptorSet, 0, nullptr);
    }

    glm::mat4 mvp = (g_frameContext.shadowPass ? g_frameContext.lightViewProjection :
                    camera.getProjectionMatrix() * camera.getViewMatrix()) * Transform::getModel();

    VkBuffer buffers[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
    vkCmdPushConstants(cmd, layout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(glm::mat4), &mvp);
    if (!g_frameContext.shadowPass)
    {
        const glm::vec4 viewModel(0.0f);
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           sizeof(glm::mat4), sizeof(glm::vec4), &viewModel);
    }
    vkCmdDraw(cmd, m_vertexCount, 1, 0, 0);
}

void Sprite3D::createVertexBuffer(VkPhysicalDevice physDevice)
{
    auto vertices = createVertices();
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS)
        throw std::runtime_error("Sprite3D: failed to create vertex buffer!");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device, m_vertexBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(physDevice, memReq.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_vertexMemory) != VK_SUCCESS)
        throw std::runtime_error("Sprite3D: failed to allocate vertex buffer memory!");

    vkBindBufferMemory(m_device, m_vertexBuffer, m_vertexMemory, 0);

    void* data;
    vkMapMemory(m_device, m_vertexMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_device, m_vertexMemory);
}

std::vector<Sprite3D::Vertex> Sprite3D::createVertices() const
{
    // clang-format off
    // Winding is CW in world space so that after the shader's Y-flip it becomes
    // CW in Vulkan framebuffer space (Y-down), matching VK_FRONT_FACE_CLOCKWISE.
    return {
        // Triangle 1
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        // Triangle 2
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},
    };
    // clang-format on
}

} // namespace spear::rendering::vulkan
