#include <spear/input.hh>

#include <utility>

namespace spear
{

void Input::sendKey(SDL_Keycode key, bool pressed, bool repeat)
{
    SDL_Event event{};
    event.type = pressed ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.key = key;
    event.key.repeat = repeat;
    sendEvent(event);
}

void Input::sendMouseMotion(float x, float y, float xrel, float yrel)
{
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.x = x;
    event.motion.y = y;
    event.motion.xrel = xrel;
    event.motion.yrel = yrel;
    sendEvent(event);
}

void Input::sendMouseButton(uint8_t button, bool pressed, float x, float y)
{
    SDL_Event event{};
    event.type = pressed ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    event.button.button = button;
    event.button.x = x;
    event.button.y = y;
    sendEvent(event);
}

void Input::sendText(std::string_view text)
{
    const std::string owned(text);
    SDL_Event event{};
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.text = owned.c_str();
    sendEvent(event);
}

bool SDLInput::pollEvent(SDL_Event& event)
{
    {
        std::lock_guard lock(m_mutex);
        if (!m_events.empty())
        {
            event = m_events.front().event;
            if (event.type == SDL_EVENT_TEXT_INPUT || event.type == SDL_EVENT_TEXT_EDITING)
            {
                m_currentText = std::move(m_events.front().text);
                if (event.type == SDL_EVENT_TEXT_INPUT)
                    event.text.text = m_currentText.c_str();
                else
                    event.edit.text = m_currentText.c_str();
            }
            m_events.pop_front();
            return true;
        }
    }
    return SDL_PollEvent(&event);
}

void SDLInput::sendEvent(const SDL_Event& event)
{
    std::lock_guard lock(m_mutex);
    QueuedEvent queued{event, {}};
    if (event.type == SDL_EVENT_TEXT_INPUT && event.text.text)
        queued.text = event.text.text;
    else if (event.type == SDL_EVENT_TEXT_EDITING && event.edit.text)
        queued.text = event.edit.text;
    m_events.push_back(std::move(queued));
}

} // namespace spear
