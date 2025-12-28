#include "Camera.h"

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Target, Up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(Fov), Aspect, Near, Far);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw -= xoffset;
    Pitch += yoffset;  // ZMIENIONE: z -= na += (odwrócenie pionowego obrotu)

    // OGRANICZENIA PITCH - zapobiegaj zaglądaniu pod teren
    if (Pitch > 85.0f)
        Pitch = 85.0f;
    if (Pitch < 5.0f)
        Pitch = 5.0f;
}

void Camera::UpdateCameraVectors(const glm::vec3& targetPos)
{
    // Oblicz pozycję kamery względem celu
    float camX = targetPos.x + Distance * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    float camY = targetPos.y + Distance * sin(glm::radians(Pitch));
    float camZ = targetPos.z + Distance * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));

    Position = glm::vec3(camX, camY, camZ);
    Target = targetPos;
}

glm::vec3 Camera::GetRayFromMouse(float mouseX, float mouseY, int screenWidth, int screenHeight) const
{
    // Konwersja współrzędnych ekranu do NDC (Normalized Device Coordinates)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    
    // Ray w przestrzeni clip
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Ray w przestrzeni eye (view)
    glm::mat4 invProj = glm::inverse(GetProjectionMatrix());
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // Ray w przestrzeni world
    glm::mat4 invView = glm::inverse(GetViewMatrix());
    glm::vec4 rayWorld = invView * rayEye;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));
    
    return rayDirection;
}
