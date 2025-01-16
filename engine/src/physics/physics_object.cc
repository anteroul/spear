#include <spear/physics/physics_object.hh>

#include <utility>

namespace spear::physics
{

PhysicsObject::PhysicsObject(float mass)
    : m_mass(mass)
{
}

PhysicsObject::PhysicsObject(PhysicsObject&& other)
    : m_mass(std::move(other.m_mass))
{
}

PhysicsObject& PhysicsObject::operator=(PhysicsObject&& other)
{
    if (this != &other)
    {
        m_mass = std::move(other.m_mass);
    }
    return *this;
}

} // namespace spear::physics
