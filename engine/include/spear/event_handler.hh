#ifndef SPEAR_EVENT_HANDLER_HH
#define SPEAR_EVENT_HANDLER_HH

#include <spear/movement_controller.hh>
#include <spear/input.hh>

#include <SDL3/SDL_events.h>

#include <functional>
#include <unordered_map>
#include <vector>

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
    void handleEvents(Input& input, MovementController& movement_controller, float delta_time,
                      bool allowMovement = true);

    /// Handle custom events from key input (fires on both press and release)
    void handleInput(SDL_Keycode key, const std::function<void()>& callback);

    /// Handle key press events (fires only on key down)
    void handleKeyPressed(SDL_Keycode key, const std::function<void()>& callback);

    /// Reset all tracked movement key states.
    void clearMovementKeys();
    bool movementKeyDown(SDL_Keycode key) const
    {
        const auto it = keyStates.find(key);
        return it != keyStates.end() && it->second;
    }

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
    std::unordered_map<SDL_Keycode, bool> keyStates = {
            {SDLK_W, false}, {SDLK_S, false}, {SDLK_A, false}, {SDLK_D, false}, {SDLK_SPACE, false}, {SDLK_LSHIFT, false}, {SDLK_RSHIFT, false}, {SDLK_LCTRL, false}, {SDLK_RCTRL, false}};
    bool running = true;
    std::vector<std::pair<SDL_Keycode, std::function<void()>>> m_inputKeys;
    std::vector<std::pair<SDL_Keycode, std::function<void()>>> m_keyPressedCallbacks;
};

} // namespace spear

#endif
