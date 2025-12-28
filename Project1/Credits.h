#pragma once
#include "SceneManager.h"

class Credits : public Scene
{
public:
	Credits(SceneManager* sm);
	void Update(float dt);
	void Render();
private:

};