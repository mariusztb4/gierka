#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "AABB.h"
#include "Vertex.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);
    ~Mesh();

    void Draw() const;
    void SetEmissiveTexture(unsigned int texID);
	void SetTexture(unsigned int texID);
    const AABB& GetAABB() const {
        return m_AABB;
	}
private:
	AABB m_AABB;

	unsigned int emissiveTextureID;  // DODAJ
	unsigned int textureID;
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    void SetupMesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);
};
