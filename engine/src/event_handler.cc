#include <spear/event_handler.hh>

namespace spear
{

/// Process SDL events and call registered callbacks
void EventHandler::handleEvents()
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
    }
}

} // namespace spear
