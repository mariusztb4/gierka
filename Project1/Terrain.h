#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "TextureLoader.h"

using namespace std;

class Camera;

class Terrain
{
public:
	Terrain();
	~Terrain();
	void Init(float sizeX = 1000.f, float sizeZ = 1000.f);
	void Render(const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& cameraPos);
	
private:
	float terrainSizeX;
	float terrainSizeZ;

	Shader shader;
	unsigned int terrainVAO;
	unsigned int terrainVBO;
	unsigned int terrainEBO;
	unsigned int terrainShaderProgram;
	unsigned int terrainTexture;
	unsigned int indexCount;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	vector<float> vertices;
};