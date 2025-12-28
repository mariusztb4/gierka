#pragma once
#include <glm/glm.hpp>

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 Center() const
    {
        return (min + max) * 0.5f;
    }

    glm::vec3 Size() const
    {
        return max - min;
    }
};