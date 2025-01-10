#include <spear/camera.hh>
#include <spear/delta_time.hh>
#include <spear/event_handler.hh>
#include <spear/model/obj_loader.hh>
#include <spear/movement_controller.hh>
#include <spear/scene_manager.hh>
#include <spear/window.hh>

#include <spear/game_object.hh>
#include <spear/rendering/opengl/model/obj_model.hh>
#include <spear/rendering/opengl/renderer.hh>
#include <spear/rendering/opengl/shader.hh>
#include <spear/rendering/opengl/shapes/cube.hh>
#include <spear/rendering/opengl/shapes/quad.hh>
#include <spear/rendering/opengl/shapes/sphere.hh>
#include <spear/rendering/opengl/texture/sdl_texture.hh>

#include <spear/physics/bullet/world.hh>

#include <iostream>

// TODO
// #include <spear/sprite_3d.hh>

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
    auto shared_bullet_world = std::make_shared<btDiscreteDynamicsWorld>(*bullet_world.getDynamicsWorld());

    using namespace spear::rendering::opengl;
    using namespace spear::physics::bullet;

    // Texture creation.
    auto niilo_texture = std::make_shared<SDLTexture>("niilo.jpg");
    auto wallnut_texture = std::make_shared<SDLTexture>("wallnut.jpg");

    spear::SceneManager scene_manager;
    spear::Scene example_scene;

    auto example_scene_container = spear::Scene::Container{
            std::make_shared<Cube>(wallnut_texture, ObjectData(shared_bullet_world, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
            std::make_shared<OBJModel>("test.obj", "test.mtl", wallnut_texture,
                                       ObjectData(shared_bullet_world, 1.0f, glm::vec3(1.0f, 5.0f, -15.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
            Cube::create(niilo_texture, ObjectData(shared_bullet_world, 1.0f, glm::vec3(1.0f, 5.0f, 13.0f), glm::vec3(1.0f, 1.0f, 1.0f))),
            std::make_shared<Quad>(ObjectData(shared_bullet_world, 1.0f, glm::vec3(2.0f, 8.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)),
            std::make_shared<Sphere>(niilo_texture, ObjectData(shared_bullet_world, 1.0f, glm::vec3(4.0f, 3.f, -4.f), glm::vec3(1.0f, 1.0f, 1.0f)))};

    example_scene_container[0]->translate(glm::vec3(0.0f, -4.0f, 0.0f));
    example_scene_container[0]->scale(glm::vec3(1000.0f, 1.0f, 1000.0f));
    example_scene.setObjects(std::move(example_scene_container));

    spear::DeltaTime delta_time_interface;
    std::unordered_map<SDL_Keycode, bool> keyStates = {
            {SDLK_W, false}, {SDLK_S, false}, {SDLK_A, false}, {SDLK_D, false}, {SDLK_SPACE, false}, {SDLK_LSHIFT, false}};

    scene_manager.addScene(std::move(example_scene));
    auto current_scene = scene_manager.getCurrentScene();
    while (true)
    {
        float delta_time = delta_time_interface.getDeltaTime();

        // Rendering.
        renderer.render();

        current_scene.update(camera);

        bullet_world.stepSimulation(1.0f / 60.f);

        // Event handling.
        eventHandler.handleEvents(movement_controller, delta_time);

        // Update SDL_Window if using OpenGL.
        window.update(gl_api);

        delta_time_interface.delay(16); // 60 fps.
    }
}
