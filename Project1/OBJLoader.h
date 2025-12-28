#pragma once
#include "Mesh.h"
#include <string>

class OBJLoader
{
public:
    // Statyczna metoda do ³adowania pliku OBJ i tworzenia Mesh*
    static Mesh* LoadOBJ(const std::string& path);
};