#pragma once
#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <assert.h>

class Texture {
public:
	Texture() = default;

	Texture(const char* image, GLenum texture, GLenum tex_wrapping, GLenum format);

	Texture(GLenum format, int img_width, int img_height, GLenum texture);

	//Texture(const char* image, const char* tex_type, GLuint slot, GLenum format, GLenum pixel_type);

	Texture(const Texture&) = default;
	Texture& operator = (const Texture&) = default;

	Texture(Texture&& move) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	void Bind();
	void Unbind();
	void Delete();

	GLuint id = -1;
	std::string type;
	std::string path;

	~Texture();
private:


	int img_width, img_height, num_col_chan;



	unsigned char* data;
};

#endif