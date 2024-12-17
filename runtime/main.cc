#include <spear/camera.hh>
#include <spear/event_handler.hh>
#include <spear/shapes/cube.hh>
#include <spear/shapes/quad.hh>
#include <spear/sprite_3d.hh>
#include <spear/window.hh>

#include <spear/rendering/opengl/renderer.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/texture.hh>

#include <iostream>

int main()
{
    const std::string window_name = "Spear application";
    const spear::Window::Size window_size = {820, 640};
    const spear::rendering::API gl_api = spear::rendering::API::OpenGL;

    spear::Window window(window_name, window_size, gl_api);
    auto w_size = window.getSize();
    spear::rendering::opengl::Renderer renderer(window.getSDLWindow());

    renderer.init();
    renderer.setViewPort(w_size.x, w_size.y);
    renderer.setBackgroundColor(0.2f, 0.3f, 0.4f, 1.0f);

    spear::Camera camera;
    spear::EventHandler eventHandler;

    eventHandler.registerCallback(SDL_EVENT_MOUSE_BUTTON_DOWN, [](const SDL_Event& event)
                                  { std::cout << "Mouse button pressed at (" << event.button.x << ", " << event.button.y << ")" << std::endl; });

    eventHandler.registerCallback(SDL_EVENT_QUIT, [](const SDL_Event&)
                                  {
        std::cout << "Quit event received. Exiting..." << std::endl;
        exit(0); });

    // Movement.
    eventHandler.registerCallback(SDL_EVENT_KEY_DOWN, [&camera](const SDL_Event& event)
                                  {
        switch (event.key.key)
        {
            case SDLK_W:
            {
                camera.moveForward(camera.getSpeed());
                std::cout << "Move W pressed" << std::endl;
                break;
            }
            case SDLK_S:
            {
                camera.moveBackward(camera.getSpeed());
                std::cout << "Move S pressed" << std::endl;
                break;
            }
            case SDLK_A:
            {
                camera.moveLeft(camera.getSpeed());
                std::cout << "Move A pressed" << std::endl;
                break;
            }
            case SDLK_D:
            {
                camera.moveRight(camera.getSpeed());
                std::cout << "Move D pressed" << std::endl;
                break;
            }
        } });

    // Update window size.
    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window](const SDL_Event&)
                                  { window.resize(); });

    spear::Cube cube(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "niilo.jpg");

    while (true)
    {
        renderer.render();
        cube.render(camera);
        eventHandler.handleEvents();
        window.update(gl_api);
    }
}
