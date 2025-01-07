#include <spear/physics/bullet/object_data.hh>

namespace spear::physics::bullet
{

ObjectData::ObjectData(std::shared_ptr<btDiscreteDynamicsWorld> world, float mass, glm::vec3 position, glm::vec3 size)
    : m_world(std::move(world)),
      m_mass(mass),
      m_position(position),
      m_size(size)
{
}

ObjectData::ObjectData(ObjectData&& other)
    : m_world(std::move(other.m_world)),
      m_mass(std::move(other.m_mass)),
      m_position(std::move(other.m_position)),
      m_size(std::move(other.m_size))
{
}

ObjectData& ObjectData::operator=(ObjectData&& other)
{
    if (this != &other)
    {
        m_world = std::move(other.m_world);
        m_mass = std::move(other.m_mass);
        m_position = std::move(other.m_position);
        m_size = std::move(other.m_size);
    }
    return *this;
}

} // namespace spear::physics::bullet
