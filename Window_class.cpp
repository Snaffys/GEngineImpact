#include"Window_class.h"

Window::Window(int width, int height){
	this->width = width;
	this->height = height;

	window = glfwCreateWindow(width, height, "Prototype", NULL, NULL);								//window creation
	assert(window != NULL && "Failed to create GLFW window");
	glfwMakeContextCurrent(window);

	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) && "Failed to initialize GLAD");
}

void Window::processInput(GLFWwindow* window) {														
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


//void Window::framebuffer_size_callback(GLFWwindow* window, int width, int heigth) {
//	glViewport(0, 0, width, heigth);
//}

//void Window::glfwSetFramebufferSizeCallback(GLFWwindow* window, framebuffer_size_callback(GLFWwindow* window, int width, int heigth));

Window::~Window() {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.7f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}