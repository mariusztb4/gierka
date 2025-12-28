#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"

class Shader;

class GameObject
{
public:
    GameObject();
    ~GameObject();

    void Init(Mesh* mesh, const glm::vec3& color);
    void Update(float dt);
    void Render(Shader& shader);

    // Transformacje
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);  // Euler angles (pitch, yaw, roll)
    glm::vec3 Scale = glm::vec3(1.0f);

    // Kolor obiektu
    glm::vec3 Color = glm::vec3(1.0f);

    // Fizyka (opcjonalnie)
    glm::vec3 Velocity = glm::vec3(0.0f);
    bool HasPhysics = false;

    Mesh* GetMesh() const { return m_Mesh; }

private:
    Mesh* m_Mesh = nullptr;
};