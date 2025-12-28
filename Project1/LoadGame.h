#pragma once
#include "Scenes.h"
#include "SceneManager.h"
#include "SceneType.h"
#include "Shader.h"
#include "LoadBG.h"
#include "LoadGame.h"
#include <mutex>

class LoadGame : public Scene
{
public:
	void OnEnter();
	void OnExit();
	void Show();
	void DisplayProgress() const;
	void Update(float dt);
	void Render() override;
	LoadGame(SceneManager* sm);
	~LoadGame();

private:
	enum LoadingStep {
		STEP_PREPARE,
		STEP_LOAD_TEXTURE_DATA,
		STEP_SIMULATE_HEAVY_WORK,
		STEP_DONE
	};

	LoadingStep currentStep = STEP_PREPARE;
	float stepProgress = 0.0f;

	Game* gameScene = nullptr;

private:
	bool glUploadDone = false;
	SceneManager* sceneManager;
	float progress = 0.0f;
	float progressSpeed = 0.3f;
	float loadingBarSpeed = 0.3f;
	float barWidth = 600.0f;
	float barHeight = 30.0f;
	float barX = (1920.0f - barWidth) / 2.0f;
	float barY = (1080.0f - barHeight) / 2.0f;

	Shader shader;
	GLuint VAO;
	GLuint VBO;
	GLuint uiShader;
	glm::mat4 u_proj;

	std::thread loadingThread;
	std::atomic<float> realProgress{ 0.0f };
	std::atomic<bool> loadingDone{ false };
	std::mutex progressMutex;

	bool uiInitialized = false;

	void InitializeLG();

	unsigned int textVAO = 0, textVBO = 0;
	unsigned int textShader = 0;
	unsigned int textTexture = 0;
	bool textInitialized = false;

	void InitializeTextRenderer();
	void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
};