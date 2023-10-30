#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <assert.h>

#include "Shader_class.h"
#include "VAO_class.h"
#include "VBO_class.h"
#include "EBO_class.h"

class Window {
public:
	Window(int width = 2400, int heigth = 1600);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void Go();

	//Window(VAO&& vao);

	~Window();
private:
	int width, heigth;
	GLFWwindow* window;

	Shader shader;
	VAO vao;
	VBO vbo;
	EBO ebo;

	void processInput(GLFWwindow* window);
};