#include "HealthBar3D.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

HealthBar3D::HealthBar3D() : VAO(0), VBO(0)
{
}

HealthBar3D::~HealthBar3D()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void HealthBar3D::Init()
{
    std::cout << "[HealthBar3D] Loading shaders..." << std::endl;
    
    // £aduj shader dla paska ¿ycia
    barShader.LoadFromFiles("shaders/healthbar.vert", "shaders/healthbar.frag");
    
    std::cout << "[HealthBar3D] Shader ID: " << barShader.ID << std::endl;
    
    if (barShader.ID == 0) {
        std::cerr << "[HealthBar3D] ERROR: Failed to load shader!" << std::endl;
    }
    
    // POPRAWIONE: Quad wyœrodkowany od -0.5 do 0.5 w obu osiach
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // lewy dolny
         0.5f, -0.5f, 0.0f,  // prawy dolny
         0.5f,  0.5f, 0.0f,  // prawy górny
        -0.5f,  0.5f, 0.0f   // lewy górny
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    std::cout << "[HealthBar3D] VAO: " << VAO << ", VBO: " << VBO << std::endl;
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    std::cout << "[HealthBar3D] Init complete" << std::endl;
}

void HealthBar3D::Render(const glm::vec3& worldPosition, float healthPercent,
                         const glm::mat4& view, const glm::mat4& projection)
{
    if (healthPercent < 0.0f) healthPercent = 0.0f;
    if (healthPercent > 1.0f) healthPercent = 1.0f;
    
    if (VAO == 0 || barShader.ID == 0) {

        return;
    }
    
    // Pozycja nad przeciwnikiem
    glm::vec3 barPos = worldPosition + glm::vec3(0.0f, 1.8f, 0.0f);
    
    // Pobierz pozycjê kamery
    glm::mat4 invView = glm::inverse(view);
    glm::vec3 cameraPos = glm::vec3(invView[3]);
    
    // Billboard - zawsze zwrócony do kamery
    glm::vec3 toCam = glm::normalize(cameraPos - barPos);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), toCam));
    glm::vec3 up = glm::cross(toCam, right);
    
    // Stwórz macierz billboard
    glm::mat4 billboardMatrix(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(toCam, 0.0f);
    billboardMatrix[3] = glm::vec4(barPos, 1.0f);
    
    float barWidth = 1.0f;
    float barHeight = 0.08f;
    
    // Zapisz stan OpenGL
    GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    barShader.Use();
    
    // SprawdŸ czy uniformy istniej¹
    GLint loc_model = glGetUniformLocation(barShader.ID, "model");
    GLint loc_view = glGetUniformLocation(barShader.ID, "view");
    GLint loc_proj = glGetUniformLocation(barShader.ID, "projection");
    GLint loc_color = glGetUniformLocation(barShader.ID, "u_color");
    
    if (loc_model == -1 || loc_view == -1 || loc_proj == -1 || loc_color == -1) {
        std::cerr << "[HealthBar3D] ERROR: Uniform locations - model:" << loc_model 
                  << " view:" << loc_view << " proj:" << loc_proj << " color:" << loc_color << std::endl;
    }
    
    // T£O PASKA
    glm::mat4 model = billboardMatrix;
    model = glm::scale(model, glm::vec3(barWidth, barHeight, 1.0f));
    
    barShader.SetMat4("model", model);
    barShader.SetMat4("view", view);
    barShader.SetMat4("projection", projection);
    barShader.SetVec4("u_color", glm::vec4(0.2f, 0.0f, 0.0f, 0.8f));
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // PASEK ¯YCIA (zielony)
    model = billboardMatrix;
    model = glm::scale(model, glm::vec3(barWidth * healthPercent, barHeight, 1.0f));
    
    barShader.SetMat4("model", model);
    barShader.SetVec4("u_color", glm::vec4(0.9f, 0.0f, 0.3f, 1.0f));
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    
    // Przywróæ stan
    glDisable(GL_BLEND);
    if (wasDepthEnabled) glEnable(GL_DEPTH_TEST);
    if (wasCullEnabled) glEnable(GL_CULL_FACE);
}