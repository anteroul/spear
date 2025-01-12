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

    /// Default constructor.
    Scene();

    /// Constructor.
    /// \param name The name of the scene.
    Scene(const std::string& name);

    /// Destructor.
    ~Scene() = default;

    /// Move constructor.
    Scene(Scene&& other) noexcept;

    /// Move assignment operator.
    Scene& operator=(Scene&& other) noexcept;

    Scene(const Scene&& other) = delete;

    Scene& operator=(const Scene& other) = delete;

    void update(Camera& camera);

    void addObject(std::shared_ptr<GameObject> game_object);
    void emplaceObjects(Scene::Container game_objects);
    void setObjects(Scene::Container game_objects);

    bool removeObject(uint64_t id);
    std::shared_ptr<GameObject> getObject(uint64_t id);

    void setName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& getName() const
    {
        return m_name;
    }

    void setId(uint64_t id)
    {
        m_id = id;
    }

    uint64_t getId() const
    {
        return m_id;
    }

private:
    std::string m_name;
    uint64_t m_id;
    Scene::Container m_gameObjects;
};

} // namespace spear

#endif
