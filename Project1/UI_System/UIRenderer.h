#pragma once
#include "../Shader.h"
#include <glm/glm.hpp>

class UIRenderer
{
public:
    void Init(int screenWidth, int screenHeight);
    void Begin();
	void DrawRect(float x, float y, float width, float height, const glm::vec4& color);
	void DrawQuad(float x, float y, float width, float height, const glm::vec4& color);
    void End();

	const glm::mat4& GetProjection() const { return m_Projection; }

private:
	glm::mat4 m_Projection;

	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	Shader m_Shader;
	Shader m_TextShader;
};