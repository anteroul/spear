#include <spear/movement_controller.hh>

namespace spear
{
MovementController::MovementController(Camera& camera)
    : m_camera(camera)
{
}

void MovementController::processInput(const std::unordered_map<SDL_Keycode, bool>& keyStates, float deltaTime)
{
    glm::vec3 direction(0.f);

    if (keyStates.at(SDLK_W))
        direction += m_camera.getFront(); // Forward
    if (keyStates.at(SDLK_S))
        direction -= m_camera.getFront(); // Backward
    if (keyStates.at(SDLK_A))
        direction -= m_camera.getRight(); // Left
    if (keyStates.at(SDLK_D))
        direction += m_camera.getRight(); // Right
    if (keyStates.at(SDLK_SPACE))
        direction += m_camera.getUp(); // Up
    if (keyStates.at(SDLK_LSHIFT))
        direction -= m_camera.getUp(); // Down

    if (glm::length(direction) > 0.0f)
    {
        direction = glm::normalize(direction);
        m_camera.setPosition(m_camera.getPosition() += direction * m_camera.getSpeed() * deltaTime);
    }
}

} // namespace spear
