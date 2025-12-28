#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    glm::vec3 Position = glm::vec3(0, 5, 10);
    glm::vec3 Target = glm::vec3(0, 0, 0);
    glm::vec3 Up = glm::vec3(0, 1, 0);

    float Fov = 45.0f;
    float Aspect = 16.0f / 9.0f;
    float Near = 0.1f;
    float Far = 1000.0f;

    float Distance = 10.0f;
    float Pitch = 20.0f;
    float Yaw = 0.0f;
    float MouseSensitivity = 0.15f;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    void ProcessMouseMovement(float xoffset, float yoffset);
    void UpdateCameraVectors(const glm::vec3& targetPos);
    
    // DODANE: Raycasting
    glm::vec3 GetRayFromMouse(float mouseX, float mouseY, int screenWidth, int screenHeight) const;
};
