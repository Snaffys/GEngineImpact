#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "iostream"

#include "Texture_class.h"

class Framebuffer {
public:
	Framebuffer() = default;
	Framebuffer(unsigned int width, unsigned int height, Texture& texture);
	Framebuffer(unsigned int width, unsigned int height, const unsigned int samples_amount, MultisampledTexture &texture);

	void BindFramebuffer();
	void UnbindFramebuffer();
	void DeleteFramebuffer();

	void BindFramebufferR();
	void BindFramebufferD();

	void BlitFramebuffer(unsigned int width, unsigned int height);

	void BindRenderbuffer();
	void UnbindRenderbuffer();
	void DeleteRenderbuffer();

	Framebuffer(Framebuffer&& move) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	~Framebuffer();
private:
	GLuint renderbuffer_id = NULL;
protected:
	GLuint framebuffer_id = NULL;
};

class ShadowFramebuffer : public Framebuffer {
public:
	ShadowFramebuffer() = default;
	ShadowFramebuffer(unsigned int width, unsigned int height, Texture& texture);
};

#endif