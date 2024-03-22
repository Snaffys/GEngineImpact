#pragma once
#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"iostream"

class Framebuffer {
public:
	Framebuffer();

	Framebuffer(Framebuffer&& move) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	~Framebuffer();
private:
	GLuint id = -1;
};

#endif