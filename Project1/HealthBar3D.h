#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

class HealthBar3D
{
public:
    HealthBar3D();
    ~HealthBar3D();
    
    void Init();
    void Render(const glm::vec3& worldPosition, float healthPercent, 
                const glm::mat4& view, const glm::mat4& projection);
    
private:
    unsigned int VAO, VBO;
    Shader barShader;
};