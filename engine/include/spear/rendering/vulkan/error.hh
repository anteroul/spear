#ifndef SPEAR_RENDERING_VULKAN_ERROR_HH
#define SPEAR_RENDERING_VULKAN_ERROR_HH

#include <iostream>
#include <vulkan/vulkan.h>

#include <spear/rendering/vulkan/renderer.hh>

namespace spear::rendering::vulkan
{

static void vkError()
{
    // TODO: Use Vulkan Validation Layer for error handling.
    if (Renderer::getInstance() == nullptr)
    {
        std::cerr << "Vulkan error code: " << VK_ERROR_INITIALIZATION_FAILED << std::endl;
    }
}

}

#endif
