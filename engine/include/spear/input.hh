#ifndef SPEAR_INPUT_HH
#define SPEAR_INPUT_HH

#include <SDL3/SDL_events.h>

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <string_view>

namespace spear
{

/// Source and sink for the SDL events consumed by an application.
class Input
{
public:
    virtual ~Input() = default;

    /// Text pointers returned by pollEvent remain valid until the next poll.
    virtual bool pollEvent(SDL_Event& event) = 0;
    /// Implementations must copy text input and editing payloads.
    virtual void sendEvent(const SDL_Event& event) = 0;

    void sendKey(SDL_Keycode key, bool pressed, bool repeat = false);
    void sendMouseMotion(float x, float y, float xrel, float yrel);
    void sendMouseButton(uint8_t button, bool pressed, float x = 0.0f, float y = 0.0f);
    void sendText(std::string_view text);
};

/// Reads injected events first, then events from SDL's event queue.
class SDLInput final : public Input
{
public:
    bool pollEvent(SDL_Event& event) override;
    void sendEvent(const SDL_Event& event) override;

private:
    struct QueuedEvent
    {
        SDL_Event event;
        std::string text;
    };

    std::mutex m_mutex;
    std::deque<QueuedEvent> m_events;
    std::string m_currentText;
};

} // namespace spear

#endif
