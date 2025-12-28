#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"

class Enemy
{
public:
    Enemy();
    
    void Init(Mesh* mesh, const glm::vec3& color, const glm::vec3& position);
    void Update(float dt);
    void Render(Shader& shader);
    void TakeDamage(float damage);
    
    // Gettery
    glm::vec3 GetPosition() const { return Position; }
    float GetHealthPercent() const { return currentHP / maxHP; }
    bool IsAlive() const { return currentHP > 0.0f; }
    
    // Publiczne pola (lub zrób gettery/settery)
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Color;
    float Rotation;
    
private:
    Mesh* m_Mesh;
    float currentHP;
    float maxHP;
};