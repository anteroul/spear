#ifndef SPEAR_PHYSICS_PHYSICS_OBJECT_HH
#define SPEAR_PHYSICS_PHYSICS_OBJECT_HH

#include <spear/transform.hh>

namespace spear::physics
{

/// Adds physics.
class PhysicsObject : public Transform
{
public:
    // Constructor.
    PhysicsObject(float mass);

    /// Move constructor.
    PhysicsObject(PhysicsObject&& other);

    /// Move assignment operator.
    PhysicsObject& operator=(PhysicsObject&& other);

    /// Deleted copy constructor.
    PhysicsObject(const PhysicsObject&) = delete;

    /// Deleted copy assignment operator.
    PhysicsObject& operator=(const PhysicsObject& other) = delete;

protected:
    float m_mass;
};

} // namespace spear::physics

#endif
