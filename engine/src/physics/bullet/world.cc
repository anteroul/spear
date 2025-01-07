#include <spear/physics/bullet/world.hh>

namespace spear::physics::bullet
{

World::World(glm::vec3 gravity)
{
    auto bullet_gravity = btVector3(gravity.x, gravity.y, gravity.z);
    m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
    m_broadphase = std::make_unique<btDbvtBroadphase>();
    m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
    m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfig.get());

    // Set global gravity
    m_dynamicsWorld->setGravity(bullet_gravity);
}

} // namespace spear::physics::bullet
