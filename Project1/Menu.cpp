#include "Menu.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "TextureLoader.h"
#include "SceneManager.h"
#include <iostream>

Menu::Menu(SceneManager* sm)
{
	sceneManager = sm;
	name = "Menu";
}

void Menu::OnEnter()
{
	float vertices[] = {
		// pos        // uv
		-1.f,  1.f,  0.f, 1.f,
		-1.f, -1.f,  0.f, 0.f,
		 1.f, -1.f,  1.f, 0.f,

		-1.f,  1.f,  0.f, 1.f,
		 1.f, -1.f,  1.f, 0.f,
		 1.f,  1.f,  1.f, 1.f
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

	m_bgTexture = TextureLoader::LoadTexture("img/tlo.png");

	shader.LoadFromFiles("shaders/bg.vert", "shaders/bg.frag");

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "u_texture"), 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float bw = 437.0f;
	float spacing = -27.0f;

	int sw, sh;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &sw, &sh);

	float centerX = (sw - bw) / 2.0f;
	
	float newGameHeight = 87.0f;
	float optionsHeight = 97.0f;
	float creditsHeight = 88.0f;
	float exitHeight = 88.0f;

	float totalHeight = newGameHeight + optionsHeight + creditsHeight + exitHeight + (spacing * 3);
	float startY = (sh / 2.0f) - (totalHeight / 2.0f);

	Button newGame = { "New Game", centerX, startY + optionsHeight + creditsHeight + exitHeight + (spacing * 3), bw, newGameHeight };
	newGame.textureNormal = 0;
	newGame.textureHover = TextureLoader::LoadTexture("img/newgame.png");
	newGame.hasHoverTexture = true;
	newGame.hasNormalTexture = false;
	buttons.AddButton(newGame);

	Button options = { "Options", centerX, startY + creditsHeight + exitHeight + (spacing * 2), bw, optionsHeight };
	options.textureHover = TextureLoader::LoadTexture("img/options.png");
	options.hasHoverTexture = true;
	buttons.AddButton(options);

	Button credits = { "Credits", centerX, startY + exitHeight + spacing, bw, creditsHeight };
	credits.textureHover = TextureLoader::LoadTexture("img/credits.png");
	credits.hasHoverTexture = true;
	buttons.AddButton(credits);

	Button exit = { "Exit", centerX, startY, bw, exitHeight };
	exit.textureHover = TextureLoader::LoadTexture("img/exit.png");
	exit.hasHoverTexture = true;
	buttons.AddButton(exit);

	buttons.SetWindow(glfwGetCurrentContext());
}

void Menu::OnExit()
{
	buttons.ClearButtons();

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteTextures(1, &m_bgTexture);
}

void Menu::Render()
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(shader.ID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bgTexture);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	buttons.RenderButtons();
}

void Menu::Update(float dt)
{
	if (sceneManager == nullptr)
	{
		std::cout << "ERROR: sceneManager is nullptr in Menu!" << std::endl;
		return;
	}

	std::string clicked = buttons.Update();

	if (!clicked.empty())
	{
		std::cout << "[Menu] Button clicked: " << clicked << std::endl;

		if (clicked == "New Game")
		{
			std::cout << "[Menu] Attempting to change scene to Game..." << std::endl;
			std::cout << "[Menu] SceneManager address: " << sceneManager << std::endl;
			sceneManager->ChangeScene(SceneType::Game);
			std::cout << "[Menu] ChangeScene called!" << std::endl;
		}
		else if (clicked == "Options")
		{
			sceneManager->ChangeScene(SceneType::Options);
		}
		else if (clicked == "Credits")
		{
			sceneManager->ChangeScene(SceneType::Credits);
		}
		else if (clicked == "Exit")
		{
			exit(0);
		}
	}
}