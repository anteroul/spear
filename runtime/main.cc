#include <spear/camera.hh>
#include <spear/delta_time.hh>
#include <spear/event_handler.hh>
#include <spear/model/obj_loader.hh>
#include <spear/movement_controller.hh>
#include <spear/window.hh>

#include <spear/sprite_3d.hh>

#include <spear/rendering/opengl/model/obj_model.hh>
#include <spear/rendering/opengl/renderer.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/shapes/cube.hh>
#include <spear/rendering/opengl/shapes/quad.hh>
#include <spear/rendering/opengl/shapes/sphere.hh>
#include <spear/rendering/opengl/texture/sdl_texture.hh>

#include <spear/physics/bullet/world.hh>

int main()
{
    const std::string window_name = "Spear application";
    const spear::Window::Size window_size = {820, 640};
    const spear::rendering::API gl_api = spear::rendering::API::OpenGL;

    spear::Window window(window_name, window_size, gl_api);
    window.hideCursor(true);
    window.setMouseGrab(true);
    window.setRelativeMouseMode(true);

    auto w_size = window.getSize();
    std::cout << "Window size x: " << w_size.x << " y: " << w_size.y << std::endl;

    spear::rendering::opengl::Renderer renderer(window.getSDLWindow());
    renderer.init();
    renderer.setViewPort(w_size.x, w_size.y);
    renderer.setBackgroundColor(0.2f, 0.3f, 0.4f, 1.0f);

    spear::Camera camera;
    spear::MovementController movement_controller(camera);
    spear::EventHandler eventHandler;

    // clang-format off
    eventHandler.registerCallback(SDL_EVENT_MOUSE_BUTTON_DOWN, [](const SDL_Event& event)
    {
        std::cout << "Mouse button pressed at (" << event.button.x << ", " << event.button.y << ")" << std::endl;
    });

    eventHandler.registerCallback(SDL_EVENT_QUIT, [](const SDL_Event&)
    {
        std::cout << "Quit event received. Exiting..." << std::endl;
        exit(0);
    });

    // Mouse movement.
    eventHandler.registerCallback(SDL_EVENT_MOUSE_MOTION, [&camera](const SDL_Event& event)
                                  { camera.rotate(event.motion.xrel, event.motion.yrel); });

    // Update window size.
    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window, &renderer](const SDL_Event&)
    {
        std::cout << "Window resized!" << std::endl;
        window.resize();
        auto w_size = window.getSize();
        renderer.setViewPort(w_size.x, w_size.y); 
    });
    // clang-format on

    // Bullet world.
    spear::physics::bullet::World bullet_world;

    using namespace spear::rendering::opengl;
    using namespace spear::physics::bullet;

    // Texture creation.
    SDLTexture niilo_texture("niilo.jpg");
    SDLTexture wallnut_texture("wallnut.jpg");

    auto blender_model_data = ObjectData(std::make_shared<btDiscreteDynamicsWorld>(*bullet_world.getDynamicsWorld()), 1.0f, glm::vec3(1.0f, 10.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    OBJModel blender_model("test.obj", "test.mtl", std::make_shared<SDLTexture>(std::move(wallnut_texture)), std::move(blender_model_data));

    blender_model.translate(glm::vec3(10.0f, 10.0f, 1.0f));

    // Cube niilo_cube(std::make_shared<SDLTexture>(std::move(niilo_texture)));
    // niilo_cube.translate(glm::vec3(1.0f, 1.0f, 1.0f));

    Cube floor(std::make_shared<SDLTexture>(std::move(wallnut_texture)));
    floor.translate(glm::vec3(0.0f, -4.0f, 0.0f));
    floor.scale(glm::vec3(1000.f, 1.0f, 1000.f));

    Quad quad(glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));
    quad.translate(glm::vec3(5.0f, 1.0f, 1.0f));

    Sphere niilo_sphere(std::make_shared<SDLTexture>(std::move(niilo_texture)));
    niilo_sphere.translate(glm::vec3(5.0f, 1.0f, 1.0f));

    spear::DeltaTime delta_time_interface;
    std::unordered_map<SDL_Keycode, bool> keyStates = {
            {SDLK_W, false}, {SDLK_S, false}, {SDLK_A, false}, {SDLK_D, false}, {SDLK_SPACE, false}, {SDLK_LSHIFT, false}};

    while (true)
    {
        float delta_time = delta_time_interface.getDeltaTime();

        blender_model.applyGravity();
        blender_model.updateGameObject();

        // niilo_cube.rotate(0.01f, glm::vec3(1.0f, 1.0f, 1.0f));
        quad.rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
        niilo_sphere.rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

        // Rendering.
        renderer.render();

        // niilo_cube.render(camera);
        floor.render(camera);
        quad.render(camera);
        niilo_sphere.render(camera);
        blender_model.render(camera);

        bullet_world.stepSimulation(1.0f / 60.f);

        // Event handling.
        eventHandler.handleEvents(movement_controller, delta_time);

        // Update SDL_Window if using OpenGL.
        window.update(gl_api);

        delta_time_interface.delay(16); // 60 fps.
    }
}
