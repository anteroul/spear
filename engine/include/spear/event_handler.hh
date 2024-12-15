#ifndef SPEAR_EVENT_HANDLER_HH
#define SPEAR_EVENT_HANDLER_HH

#include <SDL3/SDL_events.h>

#include <functional>

namespace spear
{

class EventHandler
{
public:
    using EventCallback = std::function<void(const SDL_Event&)>;

    /// Register a callback for a specific event type
    void registerCallback(uint32_t eventType, EventCallback callback)
    {
        callbacks[eventType] = callback;
    }

    /// Process SDL events and call registered callbacks
    void handleEvents();

    /// Check if the application is still running
    bool isRunning() const
    {
        return running;
    }

    /// Set running to false, which can be called in an SDL_QUIT callback
    void quit()
    {
        running = false;
    }

private:
    std::unordered_map<uint32_t, EventCallback> callbacks;
    bool running = true;
};

} // namespace spear

#endif