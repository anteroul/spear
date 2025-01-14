#include <spear/window/vulkan_window.hh>

#include <SDL3/SDL_vulkan.h>

#include <iostream>
#include <vector>

namespace spear
{

VulkanWindow::VulkanWindow(const std::string& window_name, BaseWindow::Size size)
    : BaseWindow(window_name, size),
      m_vkInstance(VK_NULL_HANDLE), m_vkSurface(VK_NULL_HANDLE)
{
    createWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
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

void VulkanWindow::initializeContext()
{
    createVulkanInstance();
    createVulkanSurface();
    std::cout << "Vulkan context initialized.\n";
}

void VulkanWindow::createVulkanInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    unsigned int extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(&extensionCount))
    {
        throw std::runtime_error("Failed to get Vulkan instance extensions: " + std::string(SDL_GetError()));
    }

    std::vector<const char*> extensions(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(&extensionCount);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
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
