#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Light.h"

class Shader;

class ShadowMap
{
public:
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
    ~ShadowMap();

    void Init();
    void BeginShadowPass(Light* light);
    void EndShadowPass();
    
    void BindShadowTexture(unsigned int textureUnit = 2);
    unsigned int GetShadowTexture() const { return m_ShadowTexture; }
    
    glm::mat4 GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }

private:
    unsigned int m_Width;
    unsigned int m_Height;
    
    unsigned int m_FBO;
    unsigned int m_ShadowTexture;
    
    glm::mat4 m_LightSpaceMatrix;
};