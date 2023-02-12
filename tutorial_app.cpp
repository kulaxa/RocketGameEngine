#include "tutorial_app.hpp"
#include "rocket_pipeline.hpp"
#include <iostream>
namespace rocket {
	void TutorialApp::run()
	{
		std::cout << "Starting Tutorial App." << std::endl;
		while (!rocketWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}
