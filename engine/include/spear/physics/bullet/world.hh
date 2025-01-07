#ifndef SPEAR_PHYSICS_BULLET_WORLD_HH
#define SPEAR_PHYSICS_BULLET_WORLD_HH

#include <spear/physics/bullet/object.hh>
#include <spear/physics/constants.hh>

#include <btBulletDynamicsCommon.h>

#include <memory>

namespace spear::physics::bullet
{

class World
{
public:
    World(glm::vec3 gravity = Constants::getGravity());

    // Simulate the physics world.
    void stepSimulation(float timeStep, int maxSubSteps = 10)
    {
        m_dynamicsWorld->stepSimulation(timeStep, maxSubSteps);
    }

    // Accessor for the Bullet world
    btDiscreteDynamicsWorld* getDynamicsWorld() const
    {
        return m_dynamicsWorld.get();
    }

private:
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;
};

} // namespace spear::physics::bullet

#endif
