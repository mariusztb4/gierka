#pragma once
#include "Mesh.h"
#include <vector>

class MeshFactory
{
public:
    // Tworzy mesh kostki (cube)
    static Mesh* CreateCube();
    
    // Mo¿esz dodaæ wiêcej kszta³tów:
    static Mesh* CreateSphere(int segments = 32);
    static Mesh* CreatePlane(float width = 1.0f, float height = 1.0f);
    static Mesh* CreateCylinder(int segments = 32);
    static Mesh* CreateCone(int segments = 32);
};