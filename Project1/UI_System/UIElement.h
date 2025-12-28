// BAZA
#pragma once
#include <glm/glm.hpp>

struct UIElement
{
	glm::vec2 position;
	glm::vec2 size;

	bool visible = true;

	virtual void Draw() = 0;
};