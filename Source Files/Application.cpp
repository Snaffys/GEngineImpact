#include "Application.h"

Application::Application() {
	// Initialize GLFW library
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW";
		exit(1);
	}
	// Version and profile of GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

Application::~Application() {
	// Destroy all remaining windows and free alocated resources
	glfwTerminate();
}