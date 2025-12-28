#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>

Light::Light(LightType type)
    : Type(type)
{
}

Light::~Light()
{
}

glm::mat4 Light::GetLightSpaceMatrix() const
{
    if (Type == LightType::Directional)
    {
        // Ortograficzna projekcja dla œwiat³a kierunkowego
        glm::mat4 lightProjection = glm::ortho(
            -ShadowOrthoSize, ShadowOrthoSize,
            -ShadowOrthoSize, ShadowOrthoSize,
            ShadowNearPlane, ShadowFarPlane
        );

        glm::mat4 lightView = glm::lookAt(
            Position,
            Position + Direction,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        return lightProjection * lightView;
    }
    else
    {
        // Perspektywiczna projekcja dla punktowego/spot
        glm::mat4 lightProjection = glm::perspective(
            glm::radians(90.0f),
            1.0f,
            ShadowNearPlane,
            ShadowFarPlane
        );

        glm::mat4 lightView = glm::lookAt(
            Position,
            Position + Direction,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        return lightProjection * lightView;
    }
}

void Light::SetShadowProperties(float nearPlane, float farPlane, float orthoSize)
{
    ShadowNearPlane = nearPlane;
    ShadowFarPlane = farPlane;
    ShadowOrthoSize = orthoSize;
}