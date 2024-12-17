#ifndef SPEAR_STB_TEXTURE_HH
#define SPEAR_STB_TEXTURE_HH

#include <cstdint>
#include <string>

namespace spear
{

struct StbTexture
{
    StbTexture(const std::string& path);
    uint32_t m_texture;
};

} // namespace spear

#endif
