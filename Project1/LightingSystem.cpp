#include "LightingSystem.h"
#include "Player.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

LightingSystem::LightingSystem()
    : m_MainLight(nullptr), m_ShadowMap(nullptr),
      m_ScreenWidth(1920), m_ScreenHeight(1080)
{
}

LightingSystem::~LightingSystem()
{
    // Cleanup lights
    for (auto light : m_Lights)
    {
        delete light;
    }
    m_Lights.clear();
    
    if (m_ShadowMap)
        delete m_ShadowMap;
}

void LightingSystem::Init()
{
    std::cout << "=== Initializing Lighting System ===" << std::endl;
    
    // Za³aduj shadow shader
    m_ShadowShader.LoadFromFiles("shaders/shadow.vert", "shaders/shadow.frag");
    std::cout << "Shadow shader loaded!" << std::endl;
    
    // Stwórz shadow map
    m_ShadowMap = new ShadowMap(2048, 2048);
    m_ShadowMap->Init();
    
    // Stwórz g³ówne œwiat³o kierunkowe (s³oñce)
    m_MainLight = new Light(LightType::Directional);
    m_MainLight->SetPosition(glm::vec3(10.0f, 20.0f, 10.0f));
    m_MainLight->SetDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    m_MainLight->SetColor(glm::vec3(1.0f, 1.0f, 0.95f));  // Lekko ciep³e œwiat³o
    m_MainLight->SetIntensity(1.0f);
    m_MainLight->SetShadowProperties(0.1f, 100.0f, 50.0f);
    
    m_Lights.push_back(m_MainLight);
    
    std::cout << "Lighting system initialized!" << std::endl;
}

void LightingSystem::SetScreenSize(int width, int height)
{
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}

void LightingSystem::AddLight(Light* light)
{
    if (light)
    {
        m_Lights.push_back(light);
    }
}

void LightingSystem::BeginFrame()
{
    // Przygotuj shadow map do renderowania
    if (m_ShadowMap && m_MainLight)
    {
        m_ShadowMap->BeginShadowPass(m_MainLight);
    }
}

void LightingSystem::RenderShadowPass(Player* player,
                                      const std::vector<GameObject*>& objects,
                                      const std::vector<Enemy*>& enemies)
{
    m_ShadowShader.Use();

    // Pobierz macierz lightSpaceMatrix z ShadowMap
    glm::mat4 lightSpaceMatrix = m_ShadowMap ? m_ShadowMap->GetLightSpaceMatrix() : glm::mat4(1.0f);
    m_ShadowShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

    // Renderuj gracza
    if (player)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, player->Position);
        model = glm::rotate(model, player->Yaw, glm::vec3(0, 1, 0));
        model = glm::scale(model, player->Scale);

        m_ShadowShader.SetMat4("model", model);
        // Player->Render() ale bez shadera - u¿yj bezpoœrednio mesh
        // Lub: player->RenderShadow(shadowShader);
    }

    // Renderuj obiekty
    for (auto obj : objects)
    {
        if (obj)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj->Position);
            model = glm::rotate(model, obj->Rotation.y, glm::vec3(0, 1, 0));
            model = glm::scale(model, obj->Scale);

            m_ShadowShader.SetMat4("model", model);
            obj->Render(m_ShadowShader);
        }
    }

    // Renderuj enemies
    for (auto enemy : enemies)
    {
        if (enemy && enemy->IsAlive())
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, enemy->Position);
            model = glm::rotate(model, enemy->Rotation, glm::vec3(0, 1, 0));
            model = glm::scale(model, enemy->Scale);

            m_ShadowShader.SetMat4("model", model);
            enemy->Render(m_ShadowShader);
        }
    }
}

void LightingSystem::RenderObjectToShadowMap(const glm::mat4& modelMatrix, Mesh* mesh)
{
    if (!mesh)
        return;
    
    m_ShadowShader.SetMat4("model", modelMatrix);
    mesh->Draw();
}

void LightingSystem::ApplyLightingToShader(Shader& shader, const Camera& camera)
{
    if (!m_MainLight)
        return;
    
    shader.Use();
    
    // Ustaw parametry œwiat³a
    shader.SetVec3("lightPos", m_MainLight->GetPosition());
    shader.SetVec3("lightDir", m_MainLight->GetDirection());
    shader.SetVec3("lightColor", m_MainLight->GetColor() * m_MainLight->GetIntensity());
    shader.SetMat4("lightSpaceMatrix", m_MainLight->GetLightSpaceMatrix());
    shader.SetVec3("viewPos", camera.Position);
    
    // Bind shadow map
    if (m_ShadowMap)
    {
        m_ShadowMap->BindShadowTexture(2);
        shader.setInt("shadowMap", 2);
    }
}

void LightingSystem::EndFrame()
{
    if (m_ShadowMap)
    {
        m_ShadowMap->EndShadowPass();
    }
    
    // Przywróæ viewport do normalnych rozmiarów
    glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
}

glm::vec3 LightingSystem::GetLightPosition() const
{
    if (m_MainLight)
        return m_MainLight->GetPosition();
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 LightingSystem::GetLightColor() const
{
    if (m_MainLight)
        return m_MainLight->GetColor();
    return glm::vec3(1.0f, 1.0f, 1.0f);
}