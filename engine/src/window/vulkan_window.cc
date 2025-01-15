#include <spear/window/vulkan_window.hh>

#include <SDL3/SDL_vulkan.h>

#include <iostream>
#include <vector>

namespace spear
{

VulkanWindow::VulkanWindow(const std::string& window_name, BaseWindow::Size size)
    : BaseWindow(window_name, size),
      m_vkSurface(VK_NULL_HANDLE)
{
    createWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    m_vkInstance = createVulkanInstance();
    createVulkanSurface();
}

VulkanWindow::~VulkanWindow()
{
    if (m_vkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
    }
    if (m_vkInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_vkInstance, nullptr);
    }
}

VkInstance VulkanWindow::createVulkanInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = getWindowName().c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "spear";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Get SDL Vulkan extensions.
    uint32_t extensionCount = 0;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    if (!extensions)
    {
        throw std::runtime_error("Failed to get Vulkan instance extensions.");
    }

    // Query supported Vulkan extensions
    uint32_t supportedExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cerr << "vkCreateInstance failed with error code: " << result << std::endl;
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    return instance;
}

void VulkanWindow::createVulkanSurface()
{
    if (!SDL_Vulkan_CreateSurface(getSDLWindow(), m_vkInstance, nullptr, &m_vkSurface))
    {
        throw std::runtime_error("Failed to create Vulkan surface: " + std::string(SDL_GetError()));
    }
}

void VulkanWindow::update()
{
}

} // namespace spear
