#ifndef SPEAR_MOVEMENT_CONTROLLER_HH
#define SPEAR_MOVEMENT_CONTROLLER_HH

#include <spear/camera.hh>

#include <SDL3/SDL_keycode.h>
#include <glm/vec3.hpp>

namespace spear
{

class MovementController
{
public:
    /// Contructor.
    MovementController(Camera& camera);

    void processInput(const std::unordered_map<SDL_Keycode, bool>& keyStates, float deltaTime);

private:
    Camera& m_camera;
};

} // namespace spear

#endif
