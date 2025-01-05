#include <spear/delta_time.hh>

#include <SDL3/SDL_timer.h>

namespace spear
{

DeltaTime::DeltaTime()
    : m_last(SDL_GetPerformanceCounter())
{
}

float DeltaTime::getDeltaTime()
{
    uint64_t now = SDL_GetPerformanceCounter();
    float delta_time = static_cast<float>(now - m_last) / SDL_GetPerformanceFrequency();
    m_last = now;
    return delta_time;
}

void DeltaTime::delay(uint32_t ms)
{
    SDL_Delay(ms);
}

}
