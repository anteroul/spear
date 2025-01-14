#include <spear/camera.hh>
#include <spear/create_scene.hh>
#include <spear/event_handler.hh>
#include <spear/model/obj_loader.hh>
#include <spear/movement_controller.hh>
#include <spear/physics/bullet/world.hh>
#include <spear/scene_manager.hh>
#include <spear/time.hh>

#include <spear/rendering/vulkan/renderer.hh>
#include <spear/window/vulkan_window.hh>

#include <iostream>

int main()
{
    const std::string window_name = "Spear application-vulkan";
    const spear::BaseWindow::Size window_size = {820, 640};
    const spear::rendering::API gl_api = spear::rendering::API::Vulkan;

    spear::VulkanWindow window(window_name, window_size);
    auto w_size = window.getSize();
    std::cout << "Window size x: " << w_size.x << " y: " << w_size.y << std::endl;

    spear::Camera camera;
    spear::MovementController movement_controller(camera);
    spear::SceneManager scene_manager;

    spear::rendering::vulkan::Renderer renderer(window);
    renderer.init();
    renderer.setViewPort(w_size.x, w_size.y);
    renderer.setBackgroundColor(0.2f, 0.3f, 0.4f, 1.0f);

    // Texture creation.
    auto default_size = glm::vec3(1.0f, 1.0f, 1.0f);

    // Scene swapping.
    spear::Time time_interface;

    spear::EventHandler eventHandler;
    eventHandler.registerCallback(SDL_EVENT_MOUSE_BUTTON_DOWN, [](const SDL_Event& event)
                                  { std::cout << "Mouse button pressed at (" << event.button.x << ", " << event.button.y << ")" << std::endl; });

    eventHandler.registerCallback(SDL_EVENT_QUIT, [](const SDL_Event&)
                                  {
        std::cout << "Quit event received. Exiting..." << std::endl;
        exit(0); });

    // Mouse movement.
    eventHandler.registerCallback(SDL_EVENT_MOUSE_MOTION, [&camera](const SDL_Event& event)
                                  { camera.rotate(event.motion.xrel, event.motion.yrel); });

    // Update window size.
    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window, &renderer](const SDL_Event&)
                                  {
        std::cout << "Window resized!" << std::endl;
        window.resize();
        auto w_size = window.getSize();
        renderer.setViewPort(w_size.x, w_size.y); });
    // clang-format on

    auto scene_objects = spear::Scene::Container{};
    auto scene_function = [](spear::Scene::Container& objects) {
    };

    auto scene_id = spear::createScene(scene_objects, scene_function, scene_manager);
    scene_manager.loadScene(scene_id);
    auto current_scene = scene_manager.getCurrentScene();

    while (true)
    {
        // Update scenes.
        auto* new_scene = scene_manager.getCurrentScene();
        if (current_scene != new_scene)
        {
            current_scene = new_scene;
        }

        float delta_time = time_interface.getDeltaTime();
        time_interface.updateFromMain(delta_time);

        // Rendering.
        renderer.render();

        // Update object's in scene.
        current_scene->update(camera);

        // Event handling.
        eventHandler.handleEvents(movement_controller, delta_time);

        // Update SDL_Window if using OpenGL.
        window.update();

        time_interface.delay(16); // 60 fps.
    }
}
