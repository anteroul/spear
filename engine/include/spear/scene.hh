#ifndef SPEAR_SCENE_HH
#define SPEAR_SCENE_HH

#include <spear/camera.hh>
#include <spear/game_object.hh>

#include <vector>

namespace spear
{

class Scene
{
public:
    using Container = std::vector<std::shared_ptr<GameObject>>;

    Scene() = default;

    void update(Camera& camera);

    void addObject(std::shared_ptr<GameObject> game_object);
    void emplaceObjects(Scene::Container game_objects);
    void setObjects(Scene::Container game_objects);

    bool removeObject(uint64_t id);
    std::shared_ptr<GameObject> getObject(uint64_t id);

private:
    Scene::Container m_gameObjects;
};

} // namespace spear

#endif
