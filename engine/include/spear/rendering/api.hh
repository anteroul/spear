#ifndef SPEAR_RENDERING_API_HH
#define SPEAR_RENDERING_API_HH

namespace spear::rendering
{

enum API
{
    OpenGL,
    Vulkan,
    DirectX12,
    Metal
};

bool isActive(API api);

} // namespace spear::rendering

#endif
