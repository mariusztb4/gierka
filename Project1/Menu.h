#pragma once
#include <vector>
#include "Buttons.h"
#include "Scenes.h"
#include "Shader.h"

class Menu : public Scene
{
public:
	Menu(SceneManager* sm);
	void OnEnter();
	void OnExit();
	void Render();
	void Update(float dt);
	void OnStartGameClicked();

private:
	Shader shader;
	unsigned int m_VAO = 0;
	unsigned int m_VBO = 0;
	unsigned int m_bgTexture = 0;
	unsigned int m_shaderProgram = 0;
	std::vector<Button> m_buttons;
	Buttons buttons;
};