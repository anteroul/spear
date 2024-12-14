#include <spear/window.hh>
#include <spear/event_handler.hh>
#include <spear/shapes/cube.hh>
#include <spear/camera.hh>
#include <spear/sprite_3d.hh>
#include <spear/shapes/quad.hh>

#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/renderer.hh>
#include <spear/rendering/opengl/texture.hh>

#include <iostream>

int main()
{
    const std::string window_name = "Spear application";
    const spear::Window::Size window_size = { 820, 640 };
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
    {
        std::cout << "Mouse button pressed at (" << event.button.x << ", " << event.button.y << ")" << std::endl;
    });

    eventHandler.registerCallback(SDL_EVENT_QUIT, [](const SDL_Event&)
    {
        std::cout << "Quit event received. Exiting..." << std::endl;
        exit(0);
    });

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
        }
    });

    // Update window size.
    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window](const SDL_Event&){ window.resize(); });

    //auto quad_shader = spear::rendering::opengl::Shader::create(spear::rendering::ShaderType::quad);
    spear::Quad quad(glm::vec3(0.5f, 0.f, 0.2f));
    quad.initialize(glm::vec3(0.0f, 0.0f, 0.0f));

    //auto sprite_shader = spear::rendering::opengl::Shader::create(spear::rendering::ShaderType::sprite3D);
    //auto shader_id = sprite_shader->getId();
    //std::cout << "Shader id: " << shader_id << std::endl;

    //auto texture = spear::rendering::opengl::Texture(shader_id);
    //texture.loadFile("niilo.jpg");
    //auto shared_texture = std::make_shared<spear::rendering::opengl::Texture>(std::move(texture));

    //spear::Sprite3D sprite(sprite_shader, shared_texture, glm::vec3(0.f, 0.f, 0.f));
    //spear::Sprite3D sprite2(sprite_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    while (true)
    {
        quad.translate(glm::vec3(1.0f, 0.0f, 0.0f));
        //sprite.render(camera);
        //sprite2.render(camera);
        renderer.render();
        //quad.render(camera);
        eventHandler.handleEvents();
        window.update(gl_api);
    }
}
