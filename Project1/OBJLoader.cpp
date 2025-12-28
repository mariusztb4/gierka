#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "Mesh.h"
#include <iostream>

Mesh* OBJLoader::LoadOBJ(const std::string& path)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[OBJLoader] ERROR: Cannot open file: " << path << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "vt")
        {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            // Znormalizuj UV do zakresu [0,1]
            tex.x = tex.x - floor(tex.x);
            tex.y = tex.y - floor(tex.y);
            texCoords.push_back(tex);
        }
        else if (prefix == "f")
        {
            std::string v[3];
            for (int i = 0; i < 3; ++i)
                iss >> v[i];

            for (int i = 0; i < 3; ++i)
            {
                std::istringstream viss(v[i]);
                std::string idxStr;
                int posIdx = 0, texIdx = 0, normIdx = 0;

                std::getline(viss, idxStr, '/');
                posIdx = std::stoi(idxStr);
                if (std::getline(viss, idxStr, '/'))
                {
                    if (!idxStr.empty())
                        texIdx = std::stoi(idxStr);
                    if (std::getline(viss, idxStr, '/'))
                        normIdx = std::stoi(idxStr);
                }

                Vertex vert{};
                vert.Position = positions[posIdx - 1];
                vert.TexCoords = texIdx > 0 ? texCoords[texIdx - 1] : glm::vec2(0.0f);
                vert.Normal = normIdx > 0 ? normals[normIdx - 1] : glm::vec3(0.0f, 0.0f, 1.0f);
                vertices.push_back(vert);
                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }

    file.close();
    
    return new Mesh(vertices, indices);
}
