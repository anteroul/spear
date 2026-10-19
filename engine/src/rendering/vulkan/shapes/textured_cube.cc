#include <spear/rendering/vulkan/frame_context.hh>
#include <spear/rendering/vulkan/shapes/textured_cube.hh>

#include <glm/mat4x4.hpp>

#include <cstring>
#include <stdexcept>

namespace spear::rendering::vulkan
{

TexturedCube::TexturedCube(VkDevice device,
                           VkPhysicalDevice physDevice,
                           std::shared_ptr<vulkan::Texture> texture,
                           VkDescriptorPool descriptorPool,
                           VkDescriptorSetLayout descriptorSetLayout,
                           physics::bullet::ObjectData&& object_data,
                           const glm::vec4& color)
    : Cube(device, physDevice, std::move(object_data), color),
      Texture(),
      m_descriptorPool(descriptorPool),
      m_texture(std::move(texture))
{
    // Initialize the Texture base with the logical device.
    Texture::m_device = device;

    // Replace the {pos, color} vertex buffer created by Cube with a {pos, uv} one.
    replaceVertexBuffer(physDevice);

    // Create a descriptor set in the Texture base, referencing the source texture.
    Texture::createDescriptorSet(descriptorPool, descriptorSetLayout,
                                 m_texture->getImageView(),
                                 m_texture->getSampler());
}

TexturedCube::~TexturedCube()
{
    if (m_descriptorSet != VK_NULL_HANDLE)
        vkFreeDescriptorSets(Texture::m_device, m_descriptorPool, 1, &m_descriptorSet);
    m_descriptorSet = VK_NULL_HANDLE;
}

void TexturedCube::render(Camera& camera)
{
    if (g_frameContext.shadowPass && !castsShadow())
        return;
    VkCommandBuffer cmd = g_frameContext.commandBuffer;
    VkPipeline pipeline = g_frameContext.shadowPass ? g_frameContext.shadowTexturedPipeline : g_frameContext.texturedPipeline;
    VkPipelineLayout layout = g_frameContext.shadowPass ? g_frameContext.shadowPipelineLayout : g_frameContext.texturedPipelineLayout;
    if (cmd == VK_NULL_HANDLE || pipeline == VK_NULL_HANDLE || layout == VK_NULL_HANDLE)
        return;

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    if (!g_frameContext.shadowPass)
    {
        Texture::bindDescriptorSet(cmd, layout, 0);
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

void TexturedCube::replaceVertexBuffer(VkPhysicalDevice physDevice)
{
    // Destroy the {pos, color} buffer created by Cube.
    if (m_vertexBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(Cube::m_device, m_vertexBuffer, nullptr);
        m_vertexBuffer = VK_NULL_HANDLE;
    }
    if (m_vertexMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(Cube::m_device, m_vertexMemory, nullptr);
        m_vertexMemory = VK_NULL_HANDLE;
    }

    auto vertices = createVertices();
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(Cube::m_device, &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS)
        throw std::runtime_error("TexturedCube: failed to create vertex buffer!");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(Cube::m_device, m_vertexBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(physDevice, memReq.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(Cube::m_device, &allocInfo, nullptr, &m_vertexMemory) != VK_SUCCESS)
        throw std::runtime_error("TexturedCube: failed to allocate vertex buffer memory!");

    vkBindBufferMemory(Cube::m_device, m_vertexBuffer, m_vertexMemory, 0);

    void* data;
    vkMapMemory(Cube::m_device, m_vertexMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(Cube::m_device, m_vertexMemory);
}

std::vector<TexturedCube::Vertex> TexturedCube::createVertices() const
{
    const float s = 1.0f;

    // clang-format off
    return {
        // Front (+z)
        {{-s,-s,+s}, {0.0f,1.0f}},  {{+s,-s,+s}, {1.0f,1.0f}},  {{+s,+s,+s}, {1.0f,0.0f}},
        {{+s,+s,+s}, {1.0f,0.0f}},  {{-s,+s,+s}, {0.0f,0.0f}},  {{-s,-s,+s}, {0.0f,1.0f}},
        // Back (-z)
        {{+s,-s,-s}, {0.0f,1.0f}},  {{-s,-s,-s}, {1.0f,1.0f}},  {{-s,+s,-s}, {1.0f,0.0f}},
        {{-s,+s,-s}, {1.0f,0.0f}},  {{+s,+s,-s}, {0.0f,0.0f}},  {{+s,-s,-s}, {0.0f,1.0f}},
        // Left (-x)
        {{-s,-s,-s}, {0.0f,1.0f}},  {{-s,-s,+s}, {1.0f,1.0f}},  {{-s,+s,+s}, {1.0f,0.0f}},
        {{-s,+s,+s}, {1.0f,0.0f}},  {{-s,+s,-s}, {0.0f,0.0f}},  {{-s,-s,-s}, {0.0f,1.0f}},
        // Right (+x)
        {{+s,-s,+s}, {0.0f,1.0f}},  {{+s,-s,-s}, {1.0f,1.0f}},  {{+s,+s,-s}, {1.0f,0.0f}},
        {{+s,+s,-s}, {1.0f,0.0f}},  {{+s,+s,+s}, {0.0f,0.0f}},  {{+s,-s,+s}, {0.0f,1.0f}},
        // Bottom (-y)
        {{-s,-s,-s}, {0.0f,0.0f}},  {{+s,-s,-s}, {1.0f,0.0f}},  {{+s,-s,+s}, {1.0f,1.0f}},
        {{+s,-s,+s}, {1.0f,1.0f}},  {{-s,-s,+s}, {0.0f,1.0f}},  {{-s,-s,-s}, {0.0f,0.0f}},
        // Top (+y)
        {{-s,+s,+s}, {0.0f,0.0f}},  {{+s,+s,+s}, {1.0f,0.0f}},  {{+s,+s,-s}, {1.0f,1.0f}},
        {{+s,+s,-s}, {1.0f,1.0f}},  {{-s,+s,-s}, {0.0f,1.0f}},  {{-s,+s,+s}, {0.0f,0.0f}},
    };
    // clang-format on
}

} // namespace spear::rendering::vulkan
