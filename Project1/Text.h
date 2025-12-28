#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character2 {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class Text {
public:
    Text() = default;
    Text(int screenWidth, int screenHeight);
    ~Text();
    bool LoadFont(const std::string& fontPath, unsigned int fontSize);
    bool Init();
    void Render();
    void SetProjection(const glm::mat4& projection);
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec4 color);
    void SetScreenSize(int width, int height);
    void RenderTextOutlined(
        const std::string& text,
        float x, float y,
        float scale,
        glm::vec3 textColor,
        glm::vec3 outlineColor,
        float thickness
    );
    float GetTextWidth(const std::string& text, float scale);

	void Begin();
	void End();

private:
    void RenderQuad(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    GLuint LoadShader(const char* vs, const char* fs);

    Shader shader;
    std::map<char, Character2> Characters;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    glm::mat4 m_Projection = glm::mat4(1.0f);
    std::map<char, Character2> m_Characters;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};