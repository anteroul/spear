#include <spear/scene.hh>
#include <spear/util.hh>

#include <algorithm>

namespace spear
{

Scene::Scene()
    : m_name(Util::generateRandomString(10)),
      m_id(Util::generateRandomUint64())
{
}

Scene::Scene(const std::string& name)
    : m_name(name),
      m_id(Util::generateRandomUint64())
{
}

Scene::Scene(Scene&& other) noexcept
    : m_name(std::move(other.m_name)),
      m_id(std::move(other.m_id)),
      m_gameObjects(std::move(other.m_gameObjects))
{
}

Scene& Scene::operator=(Scene&& other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_id = std::move(other.m_id);
        m_gameObjects = std::move(other.m_gameObjects);
    }
    return *this;
}

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
