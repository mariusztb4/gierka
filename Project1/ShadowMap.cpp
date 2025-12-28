#include "ShadowMap.h"
#include "Shader.h"
#include <iostream>

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height), m_FBO(0), m_ShadowTexture(0)
{
}

ShadowMap::~ShadowMap()
{
    if (m_ShadowTexture)
        glDeleteTextures(1, &m_ShadowTexture);
    if (m_FBO)
        glDeleteFramebuffers(1, &m_FBO);
}

void ShadowMap::Init()
{
    // Stwórz framebuffer dla shadow map
    glGenFramebuffers(1, &m_FBO);

    // Stwórz teksturê dla depth map
    glGenTextures(1, &m_ShadowTexture);
    glBindTexture(GL_TEXTURE_2D, m_ShadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Przyczep teksturê depth do framebuffera
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowTexture, 0);
    
    // Nie potrzebujemy color attachmentu
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // SprawdŸ czy framebuffer jest kompletny
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR: Shadow map framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "Shadow map initialized (" << m_Width << "x" << m_Height << ")" << std::endl;
}

void ShadowMap::BeginShadowPass(Light* light)
{
    if (!light) return;

    // Zapisz light space matrix
    m_LightSpaceMatrix = light->GetLightSpaceMatrix();

    // Bind shadow map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // W³¹cz culling frontowych œcian (aby unikn¹æ peter panning)
    glCullFace(GL_FRONT);
}

void ShadowMap::EndShadowPass()
{
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::BindShadowTexture(unsigned int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_ShadowTexture);
}