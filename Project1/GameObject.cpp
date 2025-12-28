#include "GameObject.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
    // NIE usuwaj m_Mesh - mo¿e byæ wspó³dzielony przez wiele obiektów
}

void GameObject::Init(Mesh* mesh, const glm::vec3& color)
{
    m_Mesh = mesh;
    Color = color;
}

void GameObject::Update(float dt)
{
    // Prosta fizyka (jeœli w³¹czona)
    if (HasPhysics)
    {
        Position += Velocity * dt;
    }
}

void GameObject::Render(Shader& shader)
{
    if (!m_Mesh)
    {
        std::cerr << "ERROR: GameObject mesh is NULL!" << std::endl;
        return;
    }

    // Zbuduj macierz transformacji
    glm::mat4 model = glm::mat4(1.0f);
    
    // Translacja
    model = glm::translate(model, Position);
    
    // Rotacja (Euler angles)
    model = glm::rotate(model, Rotation.x, glm::vec3(1, 0, 0));  // Pitch
    model = glm::rotate(model, Rotation.y, glm::vec3(0, 1, 0));  // Yaw
    model = glm::rotate(model, Rotation.z, glm::vec3(0, 0, 1));  // Roll
    
    // Skalowanie
    model = glm::scale(model, Scale);

    // Ustaw shadery
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetVec3("objectColor", Color);  // WA¯NE: Ustaw kolor obiektu
    shader.setInt("hasEmissive", 0);       // WA¯NE: Wy³¹cz emissive
    shader.setInt("useObjectColor", 1);    // DODAJ: Flaga do u¿ycia koloru zamiast tekstury

    // Renderuj mesh
    m_Mesh->Draw();
}