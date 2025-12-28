#include "MeshFactory.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <cmath>

Mesh* MeshFactory::CreateCube()
{
    std::vector<Vertex> vertices = {
        // Front face (Z+)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        
        // Back face (Z-)
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        
        // Top face (Y+)
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        
        // Bottom face (Y-)
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        
        // Right face (X+)
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        
        // Left face (X-)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  0, 2, 3,      // Front
        4, 5, 6,  4, 6, 7,      // Back
        8, 9, 10, 8, 10, 11,    // Top
        12, 13, 14, 12, 14, 15, // Bottom
        16, 17, 18, 16, 18, 19, // Right
        20, 21, 22, 20, 22, 23  // Left
    };

    return new Mesh(vertices, indices);
}

Mesh* MeshFactory::CreatePlane(float width, float height)
{
    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    std::vector<Vertex> vertices = {
        {{-halfW, 0.0f, -halfH}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ halfW, 0.0f, -halfH}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ halfW, 0.0f,  halfH}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-halfW, 0.0f,  halfH}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  0, 2, 3
    };

    return new Mesh(vertices, indices);
}

Mesh* MeshFactory::CreateSphere(int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float radius = 0.5f;
    const float PI = 3.14159265359f;

    // Generuj wierzcho³ki
    for (int lat = 0; lat <= segments; ++lat)
    {
        float theta = lat * PI / segments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= segments; ++lon)
        {
            float phi = lon * 2.0f * PI / segments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            glm::vec3 position(
                radius * sinTheta * cosPhi,
                radius * cosTheta,
                radius * sinTheta * sinPhi
            );

            glm::vec3 normal = glm::normalize(position);
            glm::vec2 texCoord(
                (float)lon / segments,
                (float)lat / segments
            );

            Vertex v = { position, texCoord, normal };
            vertices.push_back(v);
        }
    }

    // Generuj indeksy
    for (int lat = 0; lat < segments; ++lat)
    {
        for (int lon = 0; lon < segments; ++lon)
        {
            int first = (lat * (segments + 1)) + lon;
            int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return new Mesh(vertices, indices);
}

Mesh* MeshFactory::CreateCylinder(int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float radius = 0.5f;
    const float height = 1.0f;
    const float PI = 3.14159265359f;

    // Wierzcho³ki boczne
    for (int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glm::vec3 normal(x, 0.0f, z);
        normal = glm::normalize(normal);

        // Górny wierzcho³ek
        vertices.push_back({
            {x, height * 0.5f, z},
            {(float)i / segments, 1.0f},
            normal
        });

        // Dolny wierzcho³ek
        vertices.push_back({
            {x, -height * 0.5f, z},
            {(float)i / segments, 0.0f},
            normal
        });
    }

    // Indeksy dla boku
    for (int i = 0; i < segments; ++i)
    {
        int base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 1);

        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    return new Mesh(vertices, indices);
}

Mesh* MeshFactory::CreateCone(int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float radius = 0.5f;
    const float height = 1.0f;
    const float PI = 3.14159265359f;

    // Wierzcho³ek na szczycie
    vertices.push_back({
        {0.0f, height * 0.5f, 0.0f},
        {0.5f, 1.0f},
        {0.0f, 1.0f, 0.0f}
    });

    // Wierzcho³ki podstawy
    for (int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back({
            {x, -height * 0.5f, z},
            {(float)i / segments, 0.0f},
            {x, 0.0f, z}
        });
    }

    // Indeksy dla boku sto¿ka
    for (int i = 1; i <= segments; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    return new Mesh(vertices, indices);
}