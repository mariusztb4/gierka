#include "SceneManager.h"
#include "LoadingManager.h"
#include <iostream>

SceneManager::SceneManager(int width, int height)
	: screenWidth(width), screenHeight(height), currentScene(nullptr)
{
	currentType = SceneType::Loading;
}

SceneManager::~SceneManager()
{
	if (currentScene)
	{
		currentScene->OnExit();
		delete currentScene;
	}
}

void SceneManager::ChangeScene(SceneType newScene)
{
	std::cout << "[SceneManager] ChangeScene: " << (int)newScene << std::endl;
	
	// POPRAWKA: Specjalna obs³uga dla Game - przejdŸ przez Loading
	// ALE TYLKO jeœli nie jesteœmy ju¿ w Loading!
	if (newScene == SceneType::Game && currentType != SceneType::Loading)
	{
		std::cout << "[SceneManager] Game requested from non-Loading scene - using Loading screen" << std::endl;
		ChangeSceneWithLoading(newScene);
		return;
	}
	
	// Normalna zmiana sceny
	if (currentScene)
	{
		std::cout << "[SceneManager] Exiting current scene: " << (int)currentType << std::endl;
		currentScene->OnExit();
		delete currentScene;
		currentScene = nullptr;
	}

	currentType = newScene;
	switch (newScene)
	{
	case SceneType::Loading:
		std::cout << "Creating Loading Scene" << std::endl;
		currentScene = new Loading(this);
		break;

		/*
	case SceneType::LoadGame:
		std::cout << "Creating LoadGame Scene" << std::endl;
		currentScene = new LoadGame(this);
		break;
		*/
	case SceneType::MainMenu:
		std::cout << "Creating Menu Scene" << std::endl;
		currentScene = new Menu(this);
		break;

	case SceneType::Game:
		std::cout << "Creating Game Scene" << std::endl;
		currentScene = new Game(this, screenWidth, screenHeight);
		break;
	case SceneType::Options:
		currentScene = new Options(this);
		break;
	case SceneType::Credits:
		currentScene = new Credits(this);
		break;
	}

	if (currentScene)
	{
		std::cout << "[SceneManager] Calling OnEnter() for scene " << (int)newScene << std::endl;
		currentScene->OnEnter();
	}
}

void SceneManager::ChangeSceneWithLoading(SceneType targetScene)
{
	std::cout << "[SceneManager] ========================================" << std::endl;
	std::cout << "[SceneManager] Starting loading sequence for scene: " << (int)targetScene << std::endl;
	
	// Usuñ aktualn¹ scenê
	if (currentScene)
	{
		std::cout << "[SceneManager] Exiting current scene..." << std::endl;
		currentScene->OnExit();
		delete currentScene;
		currentScene = nullptr;
	}
	
	// Stwórz scenê Loading
	std::cout << "[SceneManager] Creating Loading scene..." << std::endl;
	Loading* loadingScene = new Loading(this);
	
	// Prze³¹cz na Loading
	currentScene = loadingScene;
	currentType = SceneType::Loading;
	
	// WA¯NE: Najpierw OnEnter (inicjalizuje VAO, shadery itp.)
	std::cout << "[SceneManager] Calling Loading::OnEnter()..." << std::endl;
	loadingScene->OnEnter();
	
	// Teraz dodaj zadania (PO OnEnter)
	std::cout << "[SceneManager] Configuring loading tasks..." << std::endl;
	LoadingManager::ConfigureLoadingFor(loadingScene, targetScene);
	
	// Na koñcu uruchom proces ³adowania
	std::cout << "[SceneManager] Starting loading process..." << std::endl;
	loadingScene->StartLoading(targetScene);
	
	std::cout << "[SceneManager] Loading scene fully activated!" << std::endl;
	std::cout << "[SceneManager] ========================================" << std::endl;
}

void SceneManager::Update(float dt)
{
	if (currentScene)
		currentScene->Update(dt);
}

void SceneManager::Render()
{
	if (currentScene)
		currentScene->Render();
}

Scene* SceneManager::GetScene(SceneType type) const
{
    auto it = scenes.find(type);
    if (it != scenes.end())
    {
        return it->second.get();
    }
    return nullptr;
}