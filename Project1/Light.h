#pragma once
#include <glm/glm.hpp>

enum class LightType
{
    Directional,  // Kierunkowe (s³oñce)
    Point,        // Punktowe (lampa)
    Spot          // Reflektorowe
};

class Light
{
public:
    Light(LightType type = LightType::Directional);
    ~Light();

    // Gettery/Settery
    void SetPosition(const glm::vec3& pos) { Position = pos; }
    void SetDirection(const glm::vec3& dir) { Direction = glm::normalize(dir); }
    void SetColor(const glm::vec3& col) { Color = col; }
    void SetIntensity(float intensity) { Intensity = intensity; }

    glm::vec3 GetPosition() const { return Position; }
    glm::vec3 GetDirection() const { return Direction; }
    glm::vec3 GetColor() const { return Color; }
    float GetIntensity() const { return Intensity; }
    LightType GetType() const { return Type; }

    // Dla shadow mapping
    glm::mat4 GetLightSpaceMatrix() const;
    void SetShadowProperties(float nearPlane, float farPlane, float orthoSize);

private:
    LightType Type;
    
    glm::vec3 Position = glm::vec3(0.0f, 20.0f, 0.0f);
    glm::vec3 Direction = glm::vec3(-0.5f, -1.0f, -0.3f);
    glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float Intensity = 1.0f;

    // Shadow mapping properties
    float ShadowNearPlane = 0.1f;
    float ShadowFarPlane = 100.0f;
    float ShadowOrthoSize = 50.0f;
};