#include <spear/physics/base_object.hh>

#include <utility>

namespace spear::physics
{

BaseObject::BaseObject(float mass)
    : m_mass(mass)
{
}

BaseObject::BaseObject(BaseObject&& other)
    : m_mass(std::move(other.m_mass))
{
}

BaseObject& BaseObject::operator=(BaseObject&& other)
{
    if (this != &other)
    {
        m_mass = std::move(other.m_mass);
    }
    return *this;
}

} // namespace spear::physics
