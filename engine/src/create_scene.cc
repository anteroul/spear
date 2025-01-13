#include <spear/create_scene.hh>

namespace spear
{

uint64_t createScene(spear::Scene::Container objects, const std::function<void(spear::Scene::Container&)> place_elements, spear::SceneManager& scene_manager)
{
    spear::Scene scene;
    uint64_t scene_id = scene.getId();
    place_elements(objects);
    scene.setObjects(std::move(objects));
    scene_manager.addScene(std::move(scene));
    return scene_id;
}

} // namespace spear
