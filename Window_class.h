#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include<iostream>
#include<assert.h>
class Window {
public:
	Window(int width = 800, int heigth = 800);

	~Window();

	//void framebuffer_size_callback(GLFWwindow* window, int width, int heigth);

	//void glfwSetFramebufferSizeCallback(GLFWwindow* window, void(*)(GLFWwindow*, int, int));

	
	
private:
	int width, height;
	GLFWwindow* window;

	void processInput(GLFWwindow* window);
};