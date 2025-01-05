#include <spear/event_handler.hh>

namespace spear
{

/// Process SDL events and call registered callbacks
void EventHandler::handleEvents(MovementController& movement_controller, float delta_time)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        auto it = callbacks.find(event.type);
        if (it != callbacks.end())
        {
            it->second(event); // Call the registered callback
        }

        // If no callback was set for SDL_QUIT, set running to false directly.
        if (event.type == SDL_EVENT_QUIT && callbacks.find(SDL_EVENT_QUIT) == callbacks.end())
        {
            running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
        {
            bool isPressed = (event.type == SDL_EVENT_KEY_DOWN);
            if (keyStates.find(event.key.key) != keyStates.end())
            {
                keyStates[event.key.key] = isPressed;
            }
        }
    }

    movement_controller.processInput(keyStates, delta_time);
}

} // namespace spear
