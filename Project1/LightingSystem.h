#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Light.h"
#include "ShadowMap.h"
#include "Shader.h"
#include "Enemy.h"

class Camera;
class Player;
class GameObject;
class Terrain;

class LightingSystem
{
public:
    LightingSystem();
    ~LightingSystem();

    void Init();
    void SetScreenSize(int width, int height);
    
    // Zarz¹dzanie œwiat³ami
    void AddLight(Light* light);
    Light* GetMainLight() const { return m_MainLight; }
    glm::vec3 GetLightPosition() const;
    glm::vec3 GetLightColor() const;

    // Renderowanie z cieniami
    void BeginFrame();
    void RenderShadowPass(Player* player, const std::vector<GameObject*>& objects, const std::vector<Enemy*>& enemies);
    void ApplyLightingToShader(Shader& shader, const Camera& camera);
    void EndFrame();
    
    // Gettery
    ShadowMap* GetShadowMap() const { return m_ShadowMap; }

private:
    void RenderObjectToShadowMap(const glm::mat4& modelMatrix, class Mesh* mesh);

    std::vector<Light*> m_Lights;
    Light* m_MainLight;  // G³ówne œwiat³o (s³oñce)
    
    ShadowMap* m_ShadowMap;
    Shader m_ShadowShader;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
};