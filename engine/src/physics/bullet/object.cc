#include <spear/physics/bullet/object.hh>
#include <spear/physics/constants.hh>

namespace spear::physics
{

Object::Object(float mass, btDiscreteDynamicsWorld* world, const btVector3& position, const btVector3& size)
    : BaseObject(mass),
      m_dynamicsWorld(world),
      m_collisionShape(std::make_unique<btBoxShape>(size))
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(position);

    // Calculate the local inertia
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f)
    {
        m_collisionShape->calculateLocalInertia(mass, localInertia);
    }

    // Create the motion state
    m_motionState = std::make_unique<btDefaultMotionState>(transform);

    // Create the rigid body construction info
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_motionState.get(), m_collisionShape.get(), localInertia);

    // Create the rigid body
    m_rigidBody = std::make_unique<btRigidBody>(rbInfo);

    // Add the rigid body to the world
    m_dynamicsWorld->addRigidBody(m_rigidBody.get());
}

Object::~Object()
{
    if (m_dynamicsWorld && m_rigidBody)
    {
        m_dynamicsWorld->removeRigidBody(m_rigidBody.get());
    }
}

void Object::applyForce(const btVector3& force)
{
    if (m_rigidBody)
    {
        m_rigidBody->applyCentralForce(force);
    }
}

void Object::applyGravity()
{
    if (m_rigidBody && !m_rigidBody->isStaticObject())
    {
        auto gravity = Constants::getGravity();
        btVector3 currentGravity = m_mass * btVector3(gravity.x, gravity.y, gravity.z);
        m_rigidBody->applyCentralForce(currentGravity);
    }
}

btVector3 Object::getPosition() const
{
    btTransform transform;
    m_rigidBody->getMotionState()->getWorldTransform(transform);
    return transform.getOrigin();
}

} // namespace spear::physics
