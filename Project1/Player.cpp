#include "Player.h"
#include "Shader.h"
#include "TextureLoader.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void Player::Init(Mesh* mesh,
    unsigned int bodyTex,
    unsigned int emissiveTex)
{
    m_Mesh = mesh;
    bodyTexture = bodyTex;
    emissiveTexture = emissiveTex;
    
    // DEBUGOWANIE: Sprawdź czy tekstury są załadowane
    std::cout << "[Player::Init] bodyTexture ID: " << bodyTexture << std::endl;
    std::cout << "[Player::Init] emissiveTexture ID: " << emissiveTexture << std::endl;
}

void Player::ProcessInput(GLFWwindow* window, float dt, Camera* camera)
{
    glm::vec3 inputDirection(0.0f);
    bool isMoving = false;

    // Pobierz wejście z klawiatury - ODWRÓCONE ZNAKI
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        inputDirection.z -= 1.0f;
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        inputDirection.z += 1.0f;
        isMoving = true;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        inputDirection.x -= 1.0f;
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        inputDirection.x += 1.0f;
        isMoving = true;
    }

    // DODANE: Skok na spację
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
    {
        jumpVelocity = jumpStrength;
        isGrounded = false;
        std::cout << "[Player] Jump!" << std::endl;
    }

    if (isMoving && camera)
    {
        // Normalizuj wejście
        if (glm::length(inputDirection) > 0.0f)
        {
            inputDirection = glm::normalize(inputDirection);
        }

        // Oblicz kierunek kamery (tylko na płaszczyźnie XZ)
        float cameraYaw = glm::radians(camera->Yaw);
        
        // Przekształć kierunek wejścia względem kamery
        glm::vec3 forward(sin(cameraYaw), 0.0f, cos(cameraYaw));
        glm::vec3 right(cos(cameraYaw), 0.0f, -sin(cameraYaw));
        
        // Oblicz finalny kierunek ruchu względem kamery
        glm::vec3 movement = forward * inputDirection.z + right * inputDirection.x;
        
        // Normalizuj finalny wektor ruchu
        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
        }

        // Oblicz docelowy Yaw (obrót) gracza w kierunku ruchu
        float targetYaw = atan2(movement.x, movement.z);
        
        // Płynna interpolacja rotacji
        float yawDiff = targetYaw - Yaw;
        
        // Normalizuj różnicę kąta do zakresu [-PI, PI]
        while (yawDiff > glm::pi<float>()) yawDiff -= 2.0f * glm::pi<float>();
        while (yawDiff < -glm::pi<float>()) yawDiff += 2.0f * glm::pi<float>();
        
        // Płynna interpolacja
        Yaw += yawDiff * RotationSpeed * dt;

        // Przesuń gracza w obliczonym kierunku (tylko X i Z, Y obsługuje grawitacja)
        Position.x += movement.x * Speed * dt;
        Position.z += movement.z * Speed * dt;
    }
}

void Player::Update(float dt)
{
    // DODANE: Fizyka grawitacji i skoku
    if (!isGrounded)
    {
        // Zastosuj grawitację
        jumpVelocity -= gravity * dt;
        
        // Aktualizuj pozycję Y
        Position.y += jumpVelocity * dt;
        
        // Sprawdź czy postać wylądowała
        if (Position.y <= groundLevel)
        {
            Position.y = groundLevel;
            jumpVelocity = 0.0f;
            isGrounded = true;
            std::cout << "[Player] Landed" << std::endl;
        }
    }
}

void Player::Render(Shader& shader)
{
    if (!m_Mesh) {
        std::cerr << "ERROR: m_Mesh is NULL!" << std::endl;
        return;
    }

    // DEBUGOWANIE: Sprawdź czy tekstura jest poprawna
    if (bodyTexture == 0) {
        std::cerr << "ERROR: bodyTexture is 0!" << std::endl;
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model = glm::rotate(model, Yaw, glm::vec3(0, 1, 0));
    model = glm::scale(model, Scale);

    shader.Use();
    shader.SetMat4("model", model);
    
    // Ustaw tryb tekstury (nie koloru)
    shader.setInt("useObjectColor", 0);
    
    // POPRAWIONE: Aktywuj teksturę i zbinduj ją
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bodyTexture);
    shader.setInt("bodyTexture", 0);
    
    // DEBUGOWANIE: Sprawdź czy bindowanie się powiodło
    GLint currentTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
    if (currentTexture != bodyTexture) {
        std::cerr << "ERROR: Texture binding failed! Expected " << bodyTexture 
                  << " but got " << currentTexture << std::endl;
    }

    m_Mesh->Draw();
}