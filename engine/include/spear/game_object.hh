#ifndef SPEAR_GAME_OBJECT_HH
#define SPEAR_GAME_OBJECT_HH

#include <spear/physics/bullet/object.hh>
#include <spear/transform.hh>

#include <memory>

namespace spear
{

class GameObject : public Transform, public physics::bullet::Object
{
public:
    /// Constructor.
    GameObject(physics::bullet::ObjectData&& object_data);

    /// Move constructor.
    GameObject(GameObject&& other);

    /// Move assignment operator.
    GameObject& operator=(GameObject&& other);

    /// Deleted copy constructor.
    GameObject(const GameObject&) = delete;

    /// Deleted copy assignment operator.
    GameObject& operator=(const GameObject&) = delete;

    void updateGameObject();
};

} // namespace spear

#endif
