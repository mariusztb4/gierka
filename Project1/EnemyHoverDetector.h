#pragma once
#include <vector>
#include <glm/glm.hpp>

class Enemy;
class Camera;
struct GLFWwindow;

class EnemyHoverDetector
{
public:
    EnemyHoverDetector();
    ~EnemyHoverDetector();
    
    // SprawdŸ czy kursor jest nad jakimœ enemy
    void Update(GLFWwindow* window, const Camera& camera, 
                const std::vector<Enemy*>& enemies, 
                int screenWidth, int screenHeight);
    
    // Czy aktualnie najechano na enemy
    bool IsHoveringEnemy() const { return isHovering; }
    
    // Pobierz enemy na który najechano (nullptr jeœli brak)
    Enemy* GetHoveredEnemy() const { return hoveredEnemy; }
    
private:
    bool isHovering = false;
    Enemy* hoveredEnemy = nullptr;
    
    // Raycasting - oblicz promieñ z pozycji myszy
    glm::vec3 GetRayFromMouse(float mouseX, float mouseY, 
                              int screenWidth, int screenHeight,
                              const Camera& camera) const;
    
    // SprawdŸ kolizjê promienia z AABB (Axis-Aligned Bounding Box)
    bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                          const glm::vec3& boxMin, const glm::vec3& boxMax,
                          float& tMin, float& tMax) const;
};