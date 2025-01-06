#ifndef SPEAR_PHYSICS_CONSTANTS_HH
#define SPEAR_PHYSICS_CONSTANTS_HH

#include <glm/vec3.hpp>

namespace spear::physics
{

class Constants
{
public:
    static void setGravity(glm::vec3 gravity)
    {
        m_gravity = gravity;
    }
    static glm::vec3 getGravity()
    {
        return m_gravity;
    }

private:
    static glm::vec3 m_gravity;
};

} // namespace spear::physics

#endif
