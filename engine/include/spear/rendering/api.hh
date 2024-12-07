#ifndef SPEAR_RENDERING_API_HH
#define SPEAR_RENDERING_API_HH

namespace spear::rendering
{

enum API
{
    OpenGL,
    Vulkan,
    DirectX,
    Metal
};

bool isActive(API api);

}

#endif
