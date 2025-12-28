#pragma once

#include <glad/glad.h>

class SceneManager;

class LoadBG
{
public:
    LoadBG(SceneManager* sm);  // konstruktor z parametrem
    ~LoadBG();

    void OnEnter();
    void Render();
    void InitializeUI();
private:
    SceneManager* sceneManager = nullptr;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_texture = 0;
    unsigned int m_shaderProgram = 0;
    bool uiInitialized = false;

 
};