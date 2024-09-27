#include"FBO_class.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, Texture& texture) {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	// Allocates memory for renderbuffer

	texture = { GL_RGBA, width, height, GL_FLOAT, GL_COLOR_ATTACHMENT0, GL_RGBA16F };

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

	UnbindFramebuffer();
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height, Texture* texture[], int textures_amount) {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	// Allocates memory for renderbuffer

	for (unsigned int i = 0; i < textures_amount; i++)
		*texture[i] = {GL_RGBA, width, height, GL_FLOAT, GL_COLOR_ATTACHMENT0 + i, GL_RGBA16F};

	glGenRenderbuffers(1, &renderbuffer_id);
	BindRenderbuffer();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	UnbindFramebuffer();
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height, const unsigned int samples_amount, MultisampledTexture* textures[], int textures_amount) {
	// Generates and binds framebuffers
	glGenFramebuffers(1, &framebuffer_id);
	BindFramebuffer();
	
	for (unsigned int i = 0; i < textures_amount; i++) {
		// Allocates memory for renderbuffer
		*textures[i] = {GL_RGBA16F, samples_amount, width, height, GL_COLOR_ATTACHMENT0 + i};
	}
	
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

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	UnbindFramebuffer();
}






//Framebuffer::Framebuffer(unsigned int width, unsigned int height, const unsigned int samples_amount, MultisampledTexture texture, int textures_amount) {
//	// Generates and binds framebuffers
//	glGenFramebuffers(1, &framebuffer_id);
//	BindFramebuffer();
//
//	for (unsigned int i = 0; i < textures_amount; i++) {
//		// Allocates memory for renderbuffer
//		texture = { GL_RGBA16F, samples_amount, width, height };
//	}
//
//	// Generates renderbuffer
//	glGenRenderbuffers(1, &renderbuffer_id);
//	BindRenderbuffer();
//	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_amount, GL_DEPTH24_STENCIL8, width, height);
//	UnbindRenderbuffer();
//	// Attaches renderbuffer object to the depth and stencil attachment of the framebuffer
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		printf("ERROR::FRAMEBUFFER::Framebuffer is not complete!");
//
//	UnbindFramebuffer();
//}


GLuint Framebuffer::GetRenderbufferId() {
	return renderbuffer_id;
}

GLuint Framebuffer::GetFramebufferId() {
	return framebuffer_id;
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