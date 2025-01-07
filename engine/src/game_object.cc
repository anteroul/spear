#include <spear/game_object.hh>

#include <iostream>

namespace spear
{

GameObject::GameObject(physics::bullet::ObjectData&& object_data)
    : physics::bullet::Object(std::move(object_data))
{
}

GameObject::GameObject(GameObject&& other)
    : Transform(std::move(other)),
      physics::bullet::Object(std::move(other))
{
}

GameObject& GameObject::operator=(GameObject&& other)
{
    if (this != &other)
    {
        Transform::operator=(std::move(other));
        physics::bullet::Object::operator=(std::move(other));
    }
    return *this;
}

void GameObject::updateGameObject()
{
    auto og_pos = Transform::getPosition();
    auto pos = physics::bullet::Object::getPosition();
}

} // namespace spear
