#ifndef SPEAR_CAMERA_HH
#define SPEAR_CAMERA_HH

#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>

namespace spear
{

class Camera
{
public:
    const float ASPECT_RATIO = 16.f / 9.f;

    /// Constructor.
    Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f),
           glm::vec3 up = glm::vec3(0.f, 1.f, 0.f),
           float yaw = -90.f,
           float pitch = 0.f,
           float movement_speed = 2.5f,
           float mouse_sentitivity = 0.1f,
           float fov = 45.0f);

    /// \defgroup CameraGetters.

    /// \ingroup CameraGetters
    glm::mat4 getViewMatrix() const;

    /// \ingroup CameraGetters
    glm::mat4 getProjectionMatrix() const;

    /// \ingroup CameraGetters
    glm::vec3 getPosition() const
    {
        return m_position;
    }

    /// \ingroup CameraGetters
    float getSpeed() const
    {
        return m_movementSpeed;
    }

    /// \ingroup CameraGetters
    glm::vec3 getWorldUp() const
    {
        return m_worldUp;
    }

    void moveForward(float delta_time)
    {
        m_position += m_front * m_movementSpeed * delta_time;
    }

    void moveBackward(float delta_time)
    {
        m_position -= m_front * m_movementSpeed * delta_time;
    }

    void moveRight(float delta_time)
    {
        m_position += m_right * m_movementSpeed * delta_time;
    }

    void moveLeft(float delta_time)
    {
        m_position -= m_right * m_movementSpeed * delta_time;
    }

    void moveUp(float delta_time)
    {
        m_position += m_up * m_movementSpeed * delta_time;
    }

    void moveDown(float delta_time)
    {
        m_position -= m_up * m_movementSpeed * delta_time;
    }

    void rotate(float x_offset, float y_offset, bool constrain_pitch = true);

    // Zoom fov.
    void zoom(float y_offset);

    void setPosition(const glm::vec3& newPosition)
    {
        m_position = newPosition;
    }

private:
    // Updates front, right, and up vectors based on updated Euler angles
    void updateCameraVectors();

private:
    // Camera Attributes.
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Euler Angles.
    float m_yaw;
    float m_pitch;

    // Camera Options.
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
};

} // namespace spear

#endif
