#include <spear/rendering/vulkan/renderer.hh>

#include <iostream>
#include <vector>

namespace spear::rendering::vulkan
{

Renderer::Renderer(VulkanWindow& vulkan_window)
    : BaseRenderer(vulkan_window),
      m_instance(vulkan_window.getVkInstance()),
      m_surface(vulkan_window.getVkSurface())
{
}

Renderer::~Renderer()
{
    cleanup();
}

void Renderer::init()
{
    createDevice();
    createSwapChain();
}

void Renderer::render()
{
}

void Renderer::createDevice()
{
    // Query physical devices and select one that supports Vulkan
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("No Vulkan-compatible GPUs found!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    // Pick the first device for simplicity
    VkPhysicalDevice physicalDevice = devices[0];
    std::cout << "Selected Vulkan physical device." << std::endl;

    // Check for device extensions
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

    // Ensure VK_KHR_swapchain is supported
    const char* requiredExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    bool swapchainSupported = false;
    for (const auto& ext : extensions)
    {
        if (strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
        {
            swapchainSupported = true;
            break;
        }
    }

    if (!swapchainSupported)
    {
        throw std::runtime_error("VK_KHR_swapchain extension not supported by the selected device.");
    }

    // Logical device creation
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0; // Replace with a proper queue family index
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan device!");
    }
    std::cout << "Vulkan logical device created." << std::endl;
}

void Renderer::createSwapChain()
{
    auto window_size = BaseRenderer::getWindow().getSize();

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = m_surface;
    swapChainCreateInfo.minImageCount = 2;
    swapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo.imageExtent = {static_cast<uint32_t>(window_size.x), static_cast<uint32_t>(window_size.y)};
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_device, &swapChainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan swap chain!");
    }
    std::cout << "Vulkan swap chain created successfully." << std::endl;
}

void Renderer::cleanup()
{
    if (m_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
    }
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
    }
}

} // namespace spear::rendering::vulkan
