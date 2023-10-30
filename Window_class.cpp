#include"Window_class.h"
#include "Shader_class.h"
#include "VAO_class.h"
#include "VBO_class.h"

Window::Window(int width, int heigth){
	this->width = width;
	this->heigth = heigth;
	window = glfwCreateWindow(width, heigth, "Prototype", NULL, NULL);
	assert(window != NULL && "Failed to create GLFW window");
	glfwMakeContextCurrent(window);

	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) && "Failed to initialize GLAD");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	shader = { "default.vert", "default.frag" };

	vbo = {
		{ -0.5f, -0.5f, 0.0f },
		{ -0.2f,  0.5f, 0.0f },
		{ 0.5f, 0.5f, 0.0f },
		{ 0.2f, -0.5f, 0.0f }
	};

	vao = VAO(vbo, 0);

	ebo = {
		{0, 1, 2},
		{2, 3, 0}
	};

	vao.Unbind();
	ebo.Unbind();	
}

void Window::processInput(GLFWwindow* window) {														
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Window::Go() {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		//color values
		glClearColor(0.69f, 0.0f, 0.2f, 1.0f);		
		glClear(GL_COLOR_BUFFER_BIT);
		
		shader.Activate();
		vao.Bind();

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

Window::~Window() {

}