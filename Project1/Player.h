#pragma once
#include <glad/glad.h>
#include "Mesh.h"
#include <glm/glm.hpp>

struct GLFWwindow;

class Shader;
class Camera;

class Player
{
public:
    void Init(Mesh* mesh,
        unsigned int bodyTex,
        unsigned int emissiveTex);
    void Update(float dt);
    void Render(Shader& shader);
    void ProcessInput(struct GLFWwindow* window, float dt, Camera* camera);

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    float Yaw = 0.0f;
    float Speed = 15.0f;
    float RotationSpeed = 4.0f;

    unsigned int bodyTexture = 0;
    unsigned int emissiveTexture = 0;

    // DODANE: Parametry skoku
    float jumpVelocity = 0.0f;
    float jumpStrength = 14.0f;    // Si³a skoku
    float gravity = 20.0f;        // Grawitacja
    float groundLevel = 0.0f;     // Poziom ziemi
    bool isGrounded = true;       // Czy postaæ jest na ziemi

private:
    Mesh* m_Mesh = nullptr;
};