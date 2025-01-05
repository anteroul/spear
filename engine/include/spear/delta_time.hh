#ifndef SPEAR_DELTA_TIME_HH
#define SPEAR_DELTA_TIME_HH

#include <cstdint>

namespace spear
{

class DeltaTime
{
public:
    /// Constructor.
    DeltaTime();

    float getDeltaTime();
    void delay(uint32_t ms);

private:
    uint64_t m_last;
};

} // namespace spear

#endif
