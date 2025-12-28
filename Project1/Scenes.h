#pragma once
#include <string>
#include <memory>
#include "Camera.h"

class SceneManager;

class Scene
{
public:
	virtual ~Scene() = default;

	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual void Update(float dt) {}
	virtual void Render() {}

protected:
	SceneManager* sceneManager = nullptr;
	std::string name;
};