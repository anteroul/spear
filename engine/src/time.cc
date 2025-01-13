#include <spear/time.hh>

#include <SDL3/SDL_timer.h>
#include <algorithm>

namespace spear
{

Time::Time()
    : m_last(SDL_GetPerformanceCounter())
{
    m_running.store(true);
    m_eventThread = std::thread([this]()
                                { threadUpdate(); });
}

Time::~Time()
{
    stopThread();
}

void Time::updateFromMain(float delta_time)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentTime += delta_time;

        for (auto& event : m_events)
        {
            // Event in period.
            if (m_currentTime >= event.start && m_currentTime <= event.end)
            {
                if (!event.active)
                {
                    event.active = true;
                    event.callback();
                }
                // Event done.
            }
            else if (m_currentTime > event.end && event.active)
            {
                event.active = false;
            }
        }

        m_events.erase(
                std::remove_if(m_events.begin(), m_events.end(), [&](const Event& e)
                               { return m_currentTime > e.end; }),
                m_events.end());

        m_condition.notify_one();
    }
}

void Time::threadUpdate()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    while (m_running.load())
    {
        m_condition.wait(lock);
    }
}

void Time::stopThread()
{
    m_running.store(false);
    m_condition.notify_all();
    if (m_eventThread.joinable())
    {
        m_eventThread.join();
    }
}

float Time::getDeltaTime()
{
    uint64_t now = SDL_GetPerformanceCounter();
    float delta_time = static_cast<float>(now - m_last) / SDL_GetPerformanceFrequency();
    m_last = now;
    return delta_time;
}

void Time::delay(uint32_t ms)
{
    SDL_Delay(ms);
}

} // namespace spear
