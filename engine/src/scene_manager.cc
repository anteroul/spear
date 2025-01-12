#include <spear/scene_manager.hh>

namespace spear
{

void SceneManager::loadScene(const std::string& name)
{
    for (auto& scene : m_scenes)
    {
        if (scene.getName() == name)
        {
            m_currentScene = &scene;
        }
    }
}

void SceneManager::loadScene(uint64_t id)
{
    for (auto& scene : m_scenes)
    {
        if (scene.getId() == id)
        {
            m_currentScene = &scene;
        }
    }
}

} // namespace spear
