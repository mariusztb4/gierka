#include "LoadingManager.h"
#include "GameResources.h"
#include "OBJLoader.h"
#include "TextureLoader.h"
#include "MeshFactory.h"
#include <iostream>
#include <thread>
#include <chrono>

void LoadingManager::ConfigureLoadingFor(Loading* loading, SceneType targetScene)
{
    if (!loading) 
    {
        std::cout << "[LoadingManager] ERROR: loading is nullptr!" << std::endl;
        return;
    }

    std::cout << "[LoadingManager] Configuring loading for scene: " << (int)targetScene << std::endl;

    // Wyczyœæ poprzednie zadania
    loading->ClearTasks();

    // Konfiguruj zadania w zale¿noœci od docelowej sceny
    switch (targetScene)
    {
    case SceneType::Game:
        SetupGameLoading(loading);
        break;

    case SceneType::MainMenu:
        SetupMenuLoading(loading);
        break;

    default:
        // Domyœlne proste ³adowanie
        loading->AddTask("Loading...", []() {
            std::cout << "[Loading] Simple loading task" << std::endl;
        });
        break;
    }

    std::cout << "[LoadingManager] Configuration complete. Tasks added: " << loading->GetTaskCount() << std::endl;
}

void LoadingManager::SetupGameLoading(Loading* loading)
{
    std::cout << "[LoadingManager] Setting up Game loading tasks..." << std::endl;

    loading->AddTask("Initializing game systems...", []() {
        std::cout << "[Task 1/8] Initializing game systems..." << std::endl;
        GameResources::GetInstance().Clear();
    });

    loading->AddTask("Loading shaders...", []() {
        std::cout << "[Task 2/8] Loading shaders..." << std::endl;
        GameResources::GetInstance().modelShader.LoadFromFiles("shaders/model.vert", "shaders/model.frag");
        GameResources::GetInstance().terrainShader.LoadFromFiles("shaders/terrain.vert", "shaders/terrain.frag");
    });

    loading->AddTask("Loading player model...", []() {
        std::cout << "[Task 3/8] Loading player model..." << std::endl;
        GameResources::GetInstance().playerMesh = OBJLoader::LoadOBJ("models/untitled/untitled.obj");
    });

    loading->AddTask("Loading textures...", []() {
        std::cout << "[Task 4/8] Loading textures..." << std::endl;
        GameResources::GetInstance().playerTexture = TextureLoader::LoadTexture("models/untitled/texture.png");
        GameResources::GetInstance().emissiveTexture = TextureLoader::LoadTexture("models/untitled/emmisive.png");
    });

    loading->AddTask("Creating meshes...", []() {
        std::cout << "[Task 5/8] Creating meshes..." << std::endl;
        GameResources::GetInstance().cubeMesh = MeshFactory::CreateCube();
    });

    loading->AddTask("Generating terrain...", []() {
        std::cout << "[Task 6/8] Generating terrain..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    loading->AddTask("Initializing lighting system...", []() {
        std::cout << "[Task 7/8] Initializing lighting system..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    loading->AddTask("Finalizing setup...", []() {
        std::cout << "[Task 8/8] Finalizing setup..." << std::endl;
        GameResources::GetInstance().isLoaded = true;
    });

    std::cout << "[LoadingManager] Game loading tasks setup complete!" << std::endl;
}

void LoadingManager::SetupMenuLoading(Loading* loading)
{
    std::cout << "[LoadingManager] Setting up Menu loading tasks..." << std::endl;

    loading->AddTask("Loading menu interface...", []() {
        std::cout << "[Task 1/4] Loading menu interface..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    loading->AddTask("Loading button textures...", []() {
        std::cout << "[Task 2/4] Loading button textures..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    loading->AddTask("Loading background...", []() {
        std::cout << "[Task 3/4] Loading background..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    loading->AddTask("Preparing UI...", []() {
        std::cout << "[Task 4/4] Preparing UI..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
}