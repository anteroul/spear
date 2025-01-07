#ifndef SPEAR_PHYSICS_BULLET_OBJECT_DATA_HH
#define SPEAR_PHYSICS_BULLET_OBJECT_DATA_HH

#include <btBulletDynamicsCommon.h>

#include <glm/vec3.hpp>

#include <memory>

namespace spear::physics::bullet
{

class ObjectData
{
public:
    /// Constructor.
    ObjectData(std::shared_ptr<btDiscreteDynamicsWorld> world, float mass, glm::vec3 position, glm::vec3 size);

    /// Move constructor.
    ObjectData(ObjectData&& other);

    /// Move assignment operator.
    ObjectData& operator=(ObjectData&& other);

    /// Deleted copy constructor.
    ObjectData(const ObjectData&) = delete;

    /// Deleted copy assignment operator.
    ObjectData& operator=(const ObjectData&) = delete;

    btDiscreteDynamicsWorld* getWorld()
    {
        return m_world.get();
    }

    float getMass() const
    {
        return m_mass;
    }

    glm::vec3 getPosition() const
    {
        return m_position;
    }

    btVector3 getPositionAsBulletVec3() const
    {
        return btVector3(m_position.x, m_position.y, m_position.z);
    }

    glm::vec3 getSize() const
    {
        return m_size;
    }

    btVector3 getSizeAsBulletVec3() const
    {
        return btVector3(m_size.x, m_size.y, m_size.z);
    }

private:
    std::shared_ptr<btDiscreteDynamicsWorld> m_world;
    float m_mass;
    glm::vec3 m_position;
    glm::vec3 m_size;
};

} // namespace spear::physics::bullet

#endif
