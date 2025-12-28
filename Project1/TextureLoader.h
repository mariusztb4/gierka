#pragma once
#include <string>
#include <glad/glad.h>

class TextureLoader
{
public:
    // Statyczna metoda do ³adowania tekstur 2D
    static unsigned int LoadTexture(const std::string& path);
};