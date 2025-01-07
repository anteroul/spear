#ifndef SPEAR_PHYSICS_BULLET_OBJECT_HH
#define SPEAR_PHYSICS_BULLET_OBJECT_HH

#include <spear/physics/base_object.hh>
#include <spear/physics/bullet/object_data.hh>

#include <btBulletDynamicsCommon.h>

#include <glm/vec3.hpp>

#include <memory>

namespace spear::physics::bullet
{

class Object : public BaseObject
{
public:
    /// Constructor.
    Object(ObjectData&& object_data);

    /// Destructor.
    ~Object();

    /// Move constuctor.
    Object(Object&& other);

    /// Move assigment operator.
    Object& operator=(Object&& other);

    /// Deleted copy constructor.
    Object(const Object&) = delete;

    /// Copy copy assigment operator.
    Object& operator=(const Object& other) = delete;

    void applyForce(const btVector3& force);
    void applyGravity();

    btVector3 getPosition() const;

private:
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;
    std::unique_ptr<btCollisionShape> m_collisionShape;
    std::unique_ptr<btDefaultMotionState> m_motionState;
    std::unique_ptr<btRigidBody> m_rigidBody;
};

} // namespace spear::physics::bullet

#endif
