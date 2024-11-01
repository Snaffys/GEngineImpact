#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

// For swapping
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <stdio.h>

#include <assert.h>


#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Texture {
public:
	Texture() = default;


	Texture(const char* image, GLenum tex_wrapping, GLenum format, bool flip = true);
	Texture(const char* image, GLenum tex_wrapping, GLenum format, GLenum internal_format, bool flip);
	Texture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget);
	Texture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget, GLenum internal_format);

	Texture(std::string filename, const std::string directory, const char* typeName, bool gamma = false);
	Texture(const unsigned int shadow_width, const unsigned int shadow_height);

	Texture(const int rows, const int cols, const char* faces);

	Texture(GLenum internal_format, const unsigned int tex_width, const unsigned int tex_height, GLenum format, GLenum type, std::vector<glm::vec3>& ssao_noise);

	//Texture(GLenum format, const unsigned int samples_amount, const unsigned int img_width, const unsigned int img_height);

	//Texture(const char* image, const char* tex_type, GLuint slot, GLenum format, GLenum pixel_type);


	Texture(const Texture&) = default;
	Texture& operator = (const Texture&) = default;

	Texture(Texture&& move) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	void Bind();
	void BindMultisample();
	void Unbind();
	void Delete();

	void SetId(GLuint id);

	GLuint GetId();

	void SetType(std::string type);

	std::string GetType();

	void SetFilename(std::string filename);

	std::string GetFilename();


	~Texture();
private:
	int img_width, img_height, num_col_chan;
	unsigned char* data;
	std::string type;
	std::string filename;
protected:
	GLuint id = NULL;
};


class MultisampledTexture : public Texture {
public:
	MultisampledTexture() = default;
	MultisampledTexture(GLenum format, const unsigned int samples_amount, const unsigned int img_width, const unsigned int img_height, GLenum textarget);
};

class ShadowTexture : public Texture{
public:
	ShadowTexture() = default;
	ShadowTexture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget);
	ShadowTexture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget, GLenum internal_format); 
};


class CubemapShadowTexture : public Texture {
public:
	CubemapShadowTexture() = default;
	CubemapShadowTexture(GLenum format, unsigned int width, unsigned int height, GLenum type, GLenum textarget);
};

#endif