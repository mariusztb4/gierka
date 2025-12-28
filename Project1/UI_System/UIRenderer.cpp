#include "UIRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <iostream>

// Struktura do przechowywania informacji o znaku
struct Character {
    unsigned int TextureID;  // ID tekstury znaku
    glm::ivec2   Size;       // Rozmiar znaku
    glm::ivec2   Bearing;    // Offset od linii bazowej do lewego górnego rogu
    unsigned int Advance;    // Offset do następnego znaku
};

static std::map<char, Character> Characters;
static unsigned int textVAO = 0, textVBO = 0;
static bool textInitialized = false;

void UIRenderer::Init(int screenWidth, int screenHeight)
{
    m_Projection = glm::ortho(
        0.0f,
        (float)screenWidth,
        (float)screenHeight,
        0.0f,
        -1.0f,
        1.0f
    );

	// Inicjalizacja bufora wierzchołków dla prostokąta

    float quadVertices[] = {
        // Pozycje    // TexCoords
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
	};

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_Shader.LoadFromFiles("shaders/ui2.vert", "shaders/ui2.frag");
	m_TextShader.LoadFromFiles("shaders/text.vert", "shaders/text.frag");
}

void UIRenderer::Begin()
{
    glDisable(GL_SCISSOR_TEST);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Shader.Use();
    m_Shader.SetMat4("projection", m_Projection);
}

void UIRenderer::DrawRect(float x, float y, float width, float height, const glm::vec4& color)
{

	m_Shader.SetVec4("color", color);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    m_Shader.SetMat4("model", model);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void UIRenderer::DrawQuad(float x, float y, float width, float height, const glm::vec4& color)
{
    // POPRAWIONE: Włącz blending i wyłącz depth test
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_Shader.Use();
    m_Shader.SetMat4("projection", m_Projection);  // ZMIENIONE: było "u_Projection"
    m_Shader.SetVec4("color", color);               // ZMIENIONE: było "u_Color"

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    m_Shader.SetMat4("model", model);              // ZMIENIONE: było "u_Model"

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);              // ZMIENIONE: było GL_TRIANGLE_STRIP, 0, 4
    glBindVertexArray(0);
}

void UIRenderer::End()
{
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}