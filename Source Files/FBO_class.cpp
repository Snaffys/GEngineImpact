#include"FBO_class.h"

Framebuffer::Framebuffer() {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &id);
	Bind();
}

// Binds framebuffers
void Framebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
// Unbinds framebuffers
void Framebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// Deletes framebuffers
void Framebuffer::Delete()
{
	glDeleteFramebuffers(1, &id);
}

Framebuffer::Framebuffer(Framebuffer&& move) noexcept {
	std::swap(id, move.id);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	std::swap(id, other.id);
	return *this;
}

Framebuffer::~Framebuffer() {
	if (id != -1) {
		glDeleteFramebuffers(1, &id);
	}
}