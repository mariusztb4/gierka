#pragma once
#include "SceneType.h"
#include "Scenes.h"
#include "Menu.h"
#include "Game.h"
#include "Options.h"
#include "Credits.h"
#include "Loading.h"
#include "LoadGame.h"
#include <map>

class Scene;

class SceneManager
{
public:
	SceneManager(int width, int height);
	~SceneManager();

	void ChangeScene(SceneType newScene);
	void ChangeSceneWithLoading(SceneType targetScene); // DODAJ TE¯ JAKO PUBLIC
	void Update(float dt);
	void Render();
	Scene* GetScene(SceneType type) const;
	int GetScreenWidth() const { return screenWidth; }
	int GetScreenHeight() const { return screenHeight; }

	Scene* currentScene = nullptr;
	SceneType currentType;

private:
	int screenWidth;
	int screenHeight;
	std::map<SceneType, std::unique_ptr<Scene>> scenes;
};