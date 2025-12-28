#include "Mesh.h"
#include <glad/glad.h>

static AABB ComputeAABB(const std::vector<Vertex>& vertices)
{
    AABB box;
    box.min = box.max = vertices[0].Position;

    for (const auto& v : vertices)
    {
        box.min = glm::min(box.min, v.Position);
        box.max = glm::max(box.max, v.Position);
    }
    return box;
}

Mesh::Mesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices), textureID(0), emissiveTextureID(0)  // Inicjalizuj
{
    indexCount = (unsigned int)indices.size();
	m_AABB = ComputeAABB(vertices);
    SetupMesh(vertices, indices);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices)
{
    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


void Mesh::Draw() const
{
    // NIE binduj tekstur tutaj - robi to Player::Render()
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, textureID);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
