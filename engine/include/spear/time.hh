#ifndef SPEAR_DELTA_TIME_HH
#define SPEAR_DELTA_TIME_HH

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

namespace spear
{

class Time
{
public:
    struct Event
    {
        float start;
        float end;
        std::function<void()> callback;
        bool active = false;
    };

    /// Constructor.
    Time();

    /// Destructor.
    ~Time();

    void addEvent(float startTime, float endTime, std::function<void()> callback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.push_back({startTime, endTime, callback, false});
    }

    float getDeltaTime();
    void delay(uint32_t ms);

    /// Run event tracker.
    void updateFromMain(float delta_time);

private:
    /// Update thread.
    void threadUpdate();

    /// Stop event tracker.
    void stopThread();

private:
    /// Last recorded time.
    uint64_t m_last;

    /// Total elapsed time.
    float m_currentTime = 0.0f;

    /// List of events.
    std::vector<Event> m_events;

    std::mutex m_mutex;
    std::thread m_eventThread;
    std::atomic<bool> m_running;
    std::condition_variable m_condition;
};

} // namespace spear

#endif
