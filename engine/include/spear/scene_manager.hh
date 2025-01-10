#ifndef SPEAR_SCENE_MANAGER_HH
#define SPEAR_SCENE_MANAGER_HH

#include <spear/scene.hh>

namespace spear
{

class SceneManager
{
public:
    SceneManager() = default;

    void addScene(Scene&& scene)
    {
        m_scenes.emplace_back(std::move(scene));
    }

    bool removeObject(const std::string& name);

    // TODO
    Scene& getCurrentScene()
    {
        return m_scenes[0];
    }

private:
    std::vector<Scene> m_scenes;
};

} // namespace spear

#endif
