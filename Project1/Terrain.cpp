#include "Terrain.h"
#include "TextureLoader.h"  // dla LoadTexturePNG
#include <glad/glad.h>
#include <iostream>

Terrain::Terrain()
{
    terrainVAO = 0;
    terrainVBO = 0;
    terrainEBO = 0;
    terrainTexture = 0;
    indexCount = 0;
}

Terrain::~Terrain()
{
    if (terrainVAO) glDeleteVertexArrays(1, &terrainVAO);
    if (terrainVBO) glDeleteBuffers(1, &terrainVBO);
    if (terrainEBO) glDeleteBuffers(1, &terrainEBO);
    if (terrainTexture) glDeleteTextures(1, &terrainTexture);
}

void Terrain::Init(float sizeX, float sizeZ)
{
    const float SIZE = 500.0f; // pó³ szerokoœci mapy -> 1000x1000

    float vertices[] = {
        //   pozycja (x,y,z)        // uv
        -SIZE, 0.0f, -SIZE,        0.0f, 0.0f,
         SIZE, 0.0f, -SIZE,        SIZE, 0.0f,
         SIZE, 0.0f,  SIZE,        SIZE, SIZE,

        -SIZE, 0.0f, -SIZE,        0.0f, 0.0f,
         SIZE, 0.0f,  SIZE,        SIZE, SIZE,
        -SIZE, 0.0f,  SIZE,        0.0f, SIZE
    };

    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);

    glBindVertexArray(terrainVAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // layout(location = 0) -> vec3 aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // layout(location = 1) -> vec2 aTexCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    terrainTexture = TextureLoader::LoadTexture("textures/grass.png");
    
    std::cout << "Terrain initialized! Texture ID: " << terrainTexture << std::endl;
}

void Terrain::Render(const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos)
{
    if (terrainTexture == 0)
    {
        std::cout << "ERROR: Terrain texture not loaded!" << std::endl;
        return;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTexture);

    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
