#include <algorithm>
#include <spear/scene.hh>

namespace spear
{

void Scene::update(Camera& camera)
{
    for (auto& g : m_gameObjects)
    {
        g->render(camera);
    }
}

void Scene::addObject(std::shared_ptr<GameObject> game_object)
{
    m_gameObjects.emplace_back(std::move(game_object));
}

void Scene::emplaceObjects(Scene::Container game_objects)
{
    m_gameObjects.insert(m_gameObjects.begin(), game_objects.end(), game_objects.end());
}

void Scene::setObjects(Scene::Container game_objects)
{
    m_gameObjects = std::move(game_objects);
}

bool Scene::removeObject(uint64_t id)
{
    auto it = std::remove_if(m_gameObjects.begin(), m_gameObjects.end(), [id](const std::shared_ptr<GameObject>& obj)
                             { return obj->getId() == id; });
    if (it != m_gameObjects.end())
    {
        m_gameObjects.erase(it, m_gameObjects.end());
        return true;
    }
    return false;
}

std::shared_ptr<GameObject> Scene::getObject(uint64_t id)
{
    for (const auto& obj : m_gameObjects)
    {
        if (obj->getId() == id)
        {
            return obj;
        }
    }
    return nullptr;
}

} // namespace spear
