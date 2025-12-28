#include "LoadBG.h"
#include "SceneManager.h"
#include "TextureLoader.h"  // zak³adam, ¿e masz tak¹ funkcjê
#include <glad/glad.h>
#include <iostream>


LoadBG::LoadBG(SceneManager* sm)
    : sceneManager(sm)
{
    std::cout << "LoadBG: konstruktor - stworzono" << std::endl;
}

LoadBG::~LoadBG()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteTextures(1, &m_texture);
    if (m_shaderProgram != 0)
        glDeleteProgram(m_shaderProgram);
}

void LoadBG::InitializeUI()
{
    if (uiInitialized) return;

    // Pe³noekranowy quad
    float vertices[] = {
        // pozycja     // UV
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Za³aduj teksturê t³a ³adowania
    m_texture = TextureLoader::LoadTexture("img/load.png");  // zmieñ na swoj¹ teksturê

    // Prosty shader (mo¿esz u¿yæ tego samego co w Menu)
    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main()
        {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fs = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D u_texture;
        void main()
        {
            FragColor = texture(u_texture, TexCoord);
        }
    )";

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs, nullptr);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs, nullptr);
    glCompileShader(fragment);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertex);
    glAttachShader(m_shaderProgram, fragment);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "u_texture"), 0);

    std::cout << "LoadBG::OnEnter() - KONIEC" << std::endl;

    uiInitialized = true;
}


void LoadBG::OnEnter()
{

}

void LoadBG::Render()
{
    InitializeUI();

    glUseProgram(m_shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}