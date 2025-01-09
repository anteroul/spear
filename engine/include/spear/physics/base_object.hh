#ifndef SPEAR_PHYSICS_BASE_OBJECT_HH
#define SPEAR_PHYSICS_BASE_OBJECT_HH

#include <spear/transform.hh>

namespace spear::physics
{

/// Adds physics.
class BaseObject : public Transform
{
public:
    // Constructor.
    BaseObject(float mass);

    /// Move constructor.
    BaseObject(BaseObject&& other);

    /// Move assignment operator.
    BaseObject& operator=(BaseObject&& other);

    /// Deleted copy constructor.
    BaseObject(const BaseObject&) = delete;

    /// Deleted copy assignment operator.
    BaseObject& operator=(const BaseObject& other) = delete;

protected:
    float m_mass;
};

} // namespace spear::physics

#endif
