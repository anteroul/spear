#include <spear/spear_vulkan.hh>

#include <cmath>
#include <iostream>

int main()
{
    const std::string window_name = "Spear application-vulkan";
    const spear::BaseWindow::Size window_size = {820, 640};

    spear::VulkanWindow window(window_name, window_size);
    window.setIcon(spear::getAssetPath("spear_icon.png"));
    auto w_size = window.getSize();
    std::cout << "Window size x: " << w_size.x << " y: " << w_size.y << std::endl;

    spear::physics::bullet::World bullet_world;
    auto shared_bullet_world = std::make_shared<btDiscreteDynamicsWorld>(*bullet_world.getDynamicsWorld());
    auto default_size = glm::vec3(1.0f, 1.0f, 1.0f);

    // Static floor for ground collision.
    btTransform floor_transform;
    floor_transform.setIdentity();
    floor_transform.setOrigin(btVector3(0.0f, -3.0f, 0.0f));
    auto floor_shape = std::make_unique<btBoxShape>(btVector3(1000.0f, 1.0f, 1000.0f));
    auto floor_motion_state = std::make_unique<btDefaultMotionState>(floor_transform);
    btRigidBody::btRigidBodyConstructionInfo floor_rb_info(0.0f, floor_motion_state.get(), floor_shape.get(), btVector3(0, 0, 0));
    auto floor_rigid_body = std::make_unique<btRigidBody>(floor_rb_info);
    shared_bullet_world->addRigidBody(floor_rigid_body.get());

    spear::Camera camera(glm::vec3(0.0f, 1.0f, 8.0f));
    spear::MovementController movement_controller(camera, shared_bullet_world.get());
    spear::SceneManager scene_manager;

    spear::rendering::vulkan::Renderer renderer(window);
    renderer.init();
    renderer.setBackgroundColor(0.1f, 0.1f, 0.15f, 1.0f);
    renderer.setCamera(&camera);

    VkDevice device = renderer.getDevice();
    VkPhysicalDevice physDevice = renderer.getPhysicalDevice();

    // --- Descriptor pool + layout (owned here, lifetime matches the app) ---
    VkDescriptorPool descriptorPool = spear::rendering::vulkan::Texture::createDescriptorPool(device, 8);
    VkDescriptorSetLayout descriptorSetLayout = spear::rendering::vulkan::Texture::createDescriptorSetLayout(device);

    // Initialize the textured pipeline using the sampler layout.
    renderer.initializeTexturedPipeline(descriptorSetLayout);

    // --- Texture ---
    auto texture = spear::rendering::vulkan::STBTexture::create(device, physDevice, renderer.getCommandPool(), renderer.getGraphicsQueue(), spear::getAssetPath("wallnut.jpg"));
    auto niilo = spear::rendering::vulkan::STBTexture::create(device, physDevice, renderer.getCommandPool(), renderer.getGraphicsQueue(), spear::getAssetPath("niilo.jpg"));

    // clang-format off
    auto scene_objects = spear::Scene::Container{
        std::make_shared<spear::rendering::vulkan::TexturedCube>(
            device, physDevice,
            texture,
            descriptorPool, descriptorSetLayout,
            spear::physics::bullet::ObjectData(shared_bullet_world, 1.0f, glm::vec3(1.5f, 0.0f, 0.0f), default_size)),
        std::make_shared<spear::rendering::vulkan::Sprite3D>(
            device, physDevice,
            niilo,
            descriptorPool, descriptorSetLayout,
            spear::physics::bullet::ObjectData(shared_bullet_world, 0.0f, glm::vec3(-1.5f, 0.0f, 0.0f), default_size)),
        std::make_shared<spear::rendering::vulkan::OBJModel>(
            device, physDevice,
            renderer.getCommandPool(), renderer.getGraphicsQueue(),
            "/cube_pets/Models/OBJ-format/animal-bunny.obj", "/cube_pets/Models/OBJ-format/animal-bunny.mtl",
            descriptorPool, descriptorSetLayout,
            spear::physics::bullet::ObjectData(shared_bullet_world, 0.0f, glm::vec3(0.0f, 0.0f, -7.0f), default_size)),
    };
    // clang-format on

    // Lighting: fit the sun's shadow map to this small scene and add two moving point lights.
    auto& lights = renderer.getLights();
    lights.setAmbient(glm::vec3(0.2f));
    auto& sun = lights.getSun();
    sun.intensity = 0.4f;
    sun.shadowCenter = glm::vec3(0.0f);
    sun.shadowExtent = 30.0f;
    sun.shadowDistance = 60.0f;
    sun.shadowFar = 150.0f;

    spear::rendering::PointLight warm_light;
    warm_light.color = glm::vec3(1.0f, 0.6f, 0.3f);
    warm_light.intensity = 25.0f;
    warm_light.range = 25.0f;
    warm_light.castsShadow = true;
    auto warm_light_id = lights.addPointLight(warm_light);

    spear::rendering::PointLight cold_light = warm_light;
    cold_light.color = glm::vec3(0.3f, 0.5f, 1.0f);
    auto cold_light_id = lights.addPointLight(cold_light);

    // Floor to catch the shadows, and small glowing cubes marking the lights.
    auto floor = std::make_shared<spear::rendering::vulkan::Cube>(
            device, physDevice, spear::physics::bullet::ObjectData(shared_bullet_world, 0.0f, glm::vec3(0.0f), default_size),
            glm::vec4(0.6f, 0.6f, 0.6f, 1.0f));
    floor->translate(glm::vec3(0.0f, -2.0f, 0.0f));
    floor->scale(glm::vec3(30.0f, 0.1f, 30.0f));
    scene_objects.push_back(floor);

    std::array<std::shared_ptr<spear::rendering::vulkan::Cube>, 2> light_markers;
    for (auto& marker : light_markers)
    {
        marker = std::make_shared<spear::rendering::vulkan::Cube>(
                device, physDevice, spear::physics::bullet::ObjectData(shared_bullet_world, 0.0f, glm::vec3(0.0f), default_size));
        marker->setEmissive(true);
        scene_objects.push_back(marker);
    }
    float light_time = 0.0f;

    auto scene_function = [](spear::Scene::Container&) {};
    auto scene_id = spear::createScene(scene_objects, scene_function, scene_manager);
    scene_manager.loadScene(scene_id);
    spear::Time time_interface;

    // clang-format off
    spear::EventHandler eventHandler;

    eventHandler.handleInput(SDLK_ESCAPE, [&device, &descriptorPool, &descriptorSetLayout]()
    {
        vkDeviceWaitIdle(device);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        exit(0);
    });

    eventHandler.registerCallback(SDL_EVENT_QUIT, [&device, &descriptorPool, &descriptorSetLayout](const SDL_Event&)
    {
        vkDeviceWaitIdle(device);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        exit(0);
    });

    eventHandler.registerCallback(SDL_EVENT_MOUSE_MOTION, [&camera](const SDL_Event& event)
                                  { camera.rotate(event.motion.xrel, event.motion.yrel); });

    eventHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED, [&window, &renderer](const SDL_Event&)
    {
        window.resize();
        auto s = window.getSize();
        renderer.setViewPort(s.x, s.y);
    });
    // clang-format on

    while (true)
    {
        float delta_time = time_interface.getDeltaTime();
        time_interface.updateFromMain(delta_time);

        eventHandler.handleEvents(movement_controller, delta_time);

        // Move the point lights and their markers.
        light_time += delta_time;
        const glm::vec3 warm_position(std::cos(light_time) * 6.0f, 2.0f, std::sin(light_time) * 6.0f);
        const glm::vec3 cold_position(std::cos(-light_time * 0.7f) * 9.0f, 3.0f, std::sin(-light_time * 0.7f) * 9.0f - 4.0f);
        lights.getPointLight(warm_light_id)->position = warm_position;
        lights.getPointLight(cold_light_id)->position = cold_position;
        light_markers[0]->setModel(glm::scale(glm::translate(glm::mat4(1.0f), warm_position), glm::vec3(0.15f)));
        light_markers[1]->setModel(glm::scale(glm::translate(glm::mat4(1.0f), cold_position), glm::vec3(0.15f)));

        renderer.setScene(scene_manager.getCurrentScene());
        renderer.render();

        shared_bullet_world->stepSimulation(1.0f / 60.f);

        window.update();

        time_interface.delay(16);
    }
}
