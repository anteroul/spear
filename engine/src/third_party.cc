// Single compilation unit for the header-only libraries in third_party/.
// Everything else includes the headers without the implementation defines.

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "../third_party/cgltf.h"
