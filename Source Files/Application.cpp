#include "Application.h"

Application::Application() {
	// Initializes GLFW library
	glfwInit();
	// Version and profile of gl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

Application::~Application() {
	// Destroys all remaining windows and frees alocated resources
	glfwTerminate();
}
