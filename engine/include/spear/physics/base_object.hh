#ifndef SPEAR_PHYSICS_BASE_OBJECT_HH
#define SPEAR_PHYSICS_BASE_OBJECT_HH

namespace spear::physics
{

class BaseObject
{
public:
    // Constructor.
    BaseObject(float mass);

protected:
    float m_mass;
};

} // namespace spear::physics

#endif
