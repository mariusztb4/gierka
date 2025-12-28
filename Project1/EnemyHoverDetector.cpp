#include "EnemyHoverDetector.h"
#include "Enemy.h"
#include "Camera.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <algorithm>

EnemyHoverDetector::EnemyHoverDetector()
    : isHovering(false), hoveredEnemy(nullptr)
{
}

EnemyHoverDetector::~EnemyHoverDetector()
{
}

void EnemyHoverDetector::Update(GLFWwindow* window, const Camera& camera,
                                 const std::vector<Enemy*>& enemies,
                                 int screenWidth, int screenHeight)
{
    // Pobierz pozycjê myszy
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // POPRAWKA: U¿yj depth buffer do sprawdzenia czy coœ jest pod kursorem
    int pixelX = (int)mouseX;
    int pixelY = screenHeight - (int)mouseY; // Odwróæ Y (OpenGL ma (0,0) na dole)
    
    // Odczytaj g³êbokoœæ z depth buffer
    float depth;
    glReadPixels(pixelX, pixelY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    
    // Jeœli depth = 1.0, nic nie jest pod kursorem (t³o)
    if (depth >= 0.9999f)
    {
        if (isHovering)
        {
            Engine::GetInstance(1920, 1080).SwitchCursor(0);
            isHovering = false;
            hoveredEnemy = nullptr;
        }
        return;
    }
    
    // Oblicz pozycjê 3D pod kursorem
    glm::vec3 screenPos = glm::vec3(
        (mouseX / screenWidth) * 2.0f - 1.0f,
        1.0f - (mouseY / screenHeight) * 2.0f,
        depth * 2.0f - 1.0f
    );
    
    glm::mat4 invVP = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix());
    glm::vec4 worldPos = invVP * glm::vec4(screenPos, 1.0f);
    worldPos /= worldPos.w;
    
    glm::vec3 clickPos = glm::vec3(worldPos);
    
    // SprawdŸ który enemy jest najbli¿ej tej pozycji
    bool foundEnemy = false;
    Enemy* closestEnemy = nullptr;
    float closestDistance = 2.0f; // Maksymalna odleg³oœæ (promieñ tolerancji)
    
    for (auto enemy : enemies)
    {
        if (enemy && enemy->IsAlive())
        {
            glm::vec3 enemyPos = enemy->GetPosition();
            glm::vec3 enemyScale = enemy->Scale;
            
            // SprawdŸ czy klikniêta pozycja jest wewn¹trz bounding box enemy
            glm::vec3 halfSize = enemyScale * 0.5f;
            glm::vec3 boxMin = enemyPos - halfSize;
            glm::vec3 boxMax = enemyPos + halfSize;
            
            // SprawdŸ czy punkt jest w bounding box
            if (clickPos.x >= boxMin.x && clickPos.x <= boxMax.x &&
                clickPos.y >= boxMin.y && clickPos.y <= boxMax.y &&
                clickPos.z >= boxMin.z && clickPos.z <= boxMax.z)
            {
                float distance = glm::length(clickPos - enemyPos);
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    closestEnemy = enemy;
                    foundEnemy = true;
                }
            }
        }
    }
    
    // Aktualizuj stan i zmieñ kursor
    if (foundEnemy && closestEnemy)
    {
        if (!isHovering || hoveredEnemy != closestEnemy)
        {
            Engine::GetInstance(1920, 1080).SwitchCursor(1);
            isHovering = true;
            hoveredEnemy = closestEnemy;
        }
    }
    else
    {
        if (isHovering)
        {
            Engine::GetInstance(1920, 1080).SwitchCursor(0);
            isHovering = false;
            hoveredEnemy = nullptr;
        }
    }
}

glm::vec3 EnemyHoverDetector::GetRayFromMouse(float mouseX, float mouseY,
                                               int screenWidth, int screenHeight,
                                               const Camera& camera) const
{
    // Konwersja wspó³rzêdnych ekranu do NDC
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    
    // Ray w przestrzeni clip
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Ray w przestrzeni eye (view)
    glm::mat4 invProj = glm::inverse(camera.GetProjectionMatrix());
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // Ray w przestrzeni world
    glm::mat4 invView = glm::inverse(camera.GetViewMatrix());
    glm::vec4 rayWorld = invView * rayEye;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));
    
    return rayDirection;
}

bool EnemyHoverDetector::RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                            const glm::vec3& boxMin, const glm::vec3& boxMax,
                                            float& tMin, float& tMax) const
{
    tMin = 0.0f;
    tMax = FLT_MAX;
    
    // SprawdŸ przeciêcie dla ka¿dej osi (X, Y, Z)
    for (int i = 0; i < 3; i++)
    {
        if (std::abs(rayDir[i]) > 0.00001f)
        {
            float t1 = (boxMin[i] - rayOrigin[i]) / rayDir[i];
            float t2 = (boxMax[i] - rayOrigin[i]) / rayDir[i];
            
            if (t1 > t2) std::swap(t1, t2);
            
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            
            if (tMin > tMax)
                return false;
        }
    }
    
    return (tMax >= tMin && tMin >= 0.0f);
}