#include"FBO_class.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, Texture& texture) {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	// Allocates memory for renderbuffer
	texture = { GL_RGB, width, height };

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

	UnbindFramebuffer();
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height, const unsigned int samples_amount, MultisampledTexture& texture){
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	// Allocates memory for renderbuffer
	texture = { GL_RGB, samples_amount, width, height };
	// Generates renderbuffer
	glGenRenderbuffers(1, &renderbuffer_id);
	BindRenderbuffer();
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_amount, GL_DEPTH24_STENCIL8, width, height);
	UnbindRenderbuffer();
	// Attaches renderbuffer object to the depth and stencil attachment of the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

	UnbindFramebuffer();
}

ShadowFramebuffer::ShadowFramebuffer(unsigned int width, unsigned int height, Texture& texture) {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	// Allocates memory for renderbuffer
	texture = {width, height };

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

	UnbindFramebuffer();
}

// Binds framebuffers
void Framebuffer::BindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}
// Unbinds framebuffers
void Framebuffer::UnbindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// Deletes framebuffers
void Framebuffer::DeleteFramebuffer()
{
	glDeleteFramebuffers(1, &framebuffer_id);
}

void Framebuffer::BindFramebufferR() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_id);
}

void Framebuffer::BindFramebufferD() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_id);
}

void Framebuffer::BlitFramebuffer(unsigned int width, unsigned int height)
{
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

// Binds renderbuffers
void Framebuffer::BindRenderbuffer() {
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
}
// Unbinds renderbuffers
void Framebuffer::UnbindRenderbuffer() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
// Deletes renderbuffers
void Framebuffer::DeleteRenderbuffer()
{
	glDeleteRenderbuffers(1, &renderbuffer_id);
}

Framebuffer::Framebuffer(Framebuffer&& move) noexcept {
	std::swap(framebuffer_id, move.framebuffer_id);
	std::swap(renderbuffer_id, move.renderbuffer_id);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	std::swap(framebuffer_id, other.framebuffer_id);
	std::swap(renderbuffer_id, other.renderbuffer_id);
	return *this;
}

Framebuffer::~Framebuffer() {
	if (framebuffer_id != -1)
		glDeleteFramebuffers(1, &framebuffer_id);
	if (renderbuffer_id != -1)
		glDeleteRenderbuffers(1, &renderbuffer_id);
}