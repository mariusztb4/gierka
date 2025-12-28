#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <memory>

// Singleton do przechowywania za³adowanych zasobów
class GameResources
{
public:
    static GameResources& GetInstance()
    {
        static GameResources instance;
        return instance;
    }

    // Zasoby gracza
    Mesh* playerMesh = nullptr;
    unsigned int playerTexture = 0;
    unsigned int emissiveTexture = 0;

    // Shadery
    Shader modelShader;
    Shader terrainShader;

    // Mesh'e
    Mesh* cubeMesh = nullptr;

    // Flaga czy zasoby s¹ za³adowane
    bool isLoaded = false;

    void Clear()
    {
        if (playerMesh)
        {
            delete playerMesh;
            playerMesh = nullptr;
        }
        if (cubeMesh)
        {
            delete cubeMesh;
            cubeMesh = nullptr;
        }
        isLoaded = false;
    }

private:
    GameResources() = default;
    ~GameResources() { Clear(); }
    GameResources(const GameResources&) = delete;
    GameResources& operator=(const GameResources&) = delete;
};