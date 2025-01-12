#ifndef SPEAR_UTIL_HH
#define SPEAR_UTIL_HH

#include <cstdint>
#include <string>

namespace spear
{

class Util
{
public:
    static uint64_t generateRandomUint64();
    static std::string generateRandomString(uint64_t n);
};

} // namespace spear

#endif
