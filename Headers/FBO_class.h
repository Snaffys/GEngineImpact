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
	Framebuffer(unsigned int width, unsigned int height, ShadowTexture& texture);
	Framebuffer(unsigned int width, unsigned int height, const unsigned int samples_amount, MultisampledTexture* textures[], int textures_amount);
	Framebuffer(unsigned int width, unsigned int height, Texture* texture[], int textures_amount);

	void BindFramebuffer();
	void UnbindFramebuffer();
	void DeleteFramebuffer();

	void BindFramebufferR();
	void BindFramebufferD();

	void BlitFramebuffer(unsigned int width, unsigned int height);

	void BindRenderbuffer();
	void UnbindRenderbuffer();
	void DeleteRenderbuffer();

	GLuint GetRenderbufferId();
	GLuint GetFramebufferId();

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

	template <typename T> inline ShadowFramebuffer(unsigned int width, unsigned int height, T& texture) {
		// Generates and binds framebuffers
		glGenFramebuffers(1, &framebuffer_id);
		BindFramebuffer();
		// Allocates memory for renderbuffer
		texture = { GL_DEPTH_COMPONENT, width, height, GL_FLOAT, GL_DEPTH_ATTACHMENT };

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

		UnbindFramebuffer();
	}
};

#endif