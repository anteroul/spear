#ifndef SPEAR_PHYSICS_BULLET_OBJECT_HH
#define SPEAR_PHYSICS_BULLET_OBJECT_HH

#include <spear/physics/base_object.hh>

#include <btBulletDynamicsCommon.h>

#include <memory>

namespace spear::physics
{

class Object : public BaseObject
{
public:
    /// Constructor.
    Object(float mass, btDiscreteDynamicsWorld* word, const btVector3& position, const btVector3& size);

    /// Destructor.
    ~Object();

    void applyForce(const btVector3& force);
    void applyGravity();

    btVector3 getPosition() const;

private:
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    std::unique_ptr<btCollisionShape> m_collisionShape;
    std::unique_ptr<btDefaultMotionState> m_motionState;
    std::unique_ptr<btRigidBody> m_rigidBody;
};

} // namespace spear::physics

#endif
