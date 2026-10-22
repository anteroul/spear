#ifndef SPEAR_RENDERING_SHADER_INCLUDE_HH
#define SPEAR_RENDERING_SHADER_INCLUDE_HH

#include <string>

namespace spear::rendering
{

/// Replaces `#include "file.glsl"` lines with the contents of that file from the shader directory.
std::string resolveShaderIncludes(const std::string& source);

} // namespace spear::rendering

#endif
