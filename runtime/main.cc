#include <spear/camera.hh>
#include <spear/event_handler.hh>
#include <spear/sprite_3d.hh>
#include <spear/window.hh>

#include <spear/rendering/opengl/renderer.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/shapes/cube.hh>

int main()
{
    const std::string window_name = "Spear application";
    const spear::Window::Size window_size = {820, 640};
    const spear::rendering::API gl_api = spear::rendering::API::OpenGL;

    spear::Window window(window_name, window_size, gl_api);
    auto w_size = window.getSize();
    std::cout << "Window size x: " << w_size.x << " y: " << w_size.y << std::endl;

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
    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window, &renderer](const SDL_Event&)
                                  {
                                        std::cout << "Window resized!" << std::endl;
                                        window.resize();
                                        auto w_size = window.getSize();
                                        renderer.setViewPort(w_size.x, w_size.y); });

    spear::rendering::opengl::SDLTexture texture("niilo.jpg");

    spear::Cube cube(std::make_shared<spear::rendering::opengl::SDLTexture>(std::move(texture)));

    while (true)
    {
        renderer.render();
        cube.rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
        cube.rotate(0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
        cube.rotate(0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
        cube.render(camera);
        eventHandler.handleEvents();
        window.update(gl_api);
    }
}
