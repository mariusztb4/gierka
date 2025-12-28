#include <iostream>
#include "Engine.h"

int main() {

	std::cout << "Start programu" << std::endl;

	int screenWidth = 1920;
	int screenHeight = 1080;

	Engine& engine = Engine::GetInstance(screenWidth, screenHeight);

	if(!engine.Init()) {
		return -1;
	}

	engine.Run();


	return 0;
}