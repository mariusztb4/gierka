#include "Enemy.h"
#include <glm/gtc/matrix_transform.hpp>

Enemy::Enemy()
    : Position(0.0f), Scale(1.0f), Color(1.0f, 0.0f, 0.0f),
      Rotation(0.0f), m_Mesh(nullptr), currentHP(100.0f), maxHP(100.0f)
{
}

void Enemy::Init(Mesh* mesh, const glm::vec3& color, const glm::vec3& position)
{
    m_Mesh = mesh;
    Color = color;
    Position = position;
    Scale = glm::vec3(1.0f);
    currentHP = maxHP;
}

void Enemy::Update(float dt)
{
    // Mo¿esz dodaæ animacje, AI, patrol itp.
    // Na przyk³ad: powolna rotacja
    Rotation += dt * 0.5f;
}

void Enemy::Render(Shader& shader)
{
    if (!m_Mesh) return;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model = glm::rotate(model, Rotation, glm::vec3(0, 1, 0));
    model = glm::scale(model, Scale);
    
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetVec3("objectColor", Color);
    shader.setInt("useObjectColor", 1);
    
    m_Mesh->Draw();
}

void Enemy::TakeDamage(float damage)
{
    currentHP -= damage;
    if (currentHP < 0.0f)
        currentHP = 0.0f;
}