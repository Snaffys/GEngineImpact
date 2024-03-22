#include "Texture_class.h"

Texture::Texture(const char* image, GLenum texture, GLenum tex_wrapping, GLenum format) {
	// Generates textures names, activates and binds textures
	glGenTextures(1, &id);
	glActiveTexture(texture);
	Bind();

	// Texture stretching
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_wrapping);

	// Texture filtering(blurred or pixeled)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// zoomed out so far
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// zoomed in so close

	// Texture flip
	stbi_set_flip_vertically_on_load(true);
	// gets data about texture from file
	data = stbi_load(image, &img_width, &img_height, &num_col_chan, 0);
	// Checks for errors
	assert(data && "Failed to load texture");
	// generates currently bounded texture
	glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, data);
	// generates mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// frees data
	stbi_image_free(data);
}

Texture::Texture(GLenum format, int img_width, int img_height, GLenum texture) {
	glGenTextures(1, &id);
	glActiveTexture(texture);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
}


//Texture::Texture(const char* image, const char* tex_type, GLuint slot, GLenum format, GLenum pixel_type) {
//	type = GL_TEXTURE_2D;
//
//	// Texture flip
//	stbi_set_flip_vertically_on_load(true);
//	// reads the image from a file and stores it in bytes
//	data = stbi_load(image, &img_width, &img_height, &num_col_chan, 0);
//
//	// Generates, activates and binds textures
//	glGenTextures(1, &id);
//	glActiveTexture(GL_TEXTURE0 + slot);
//	//unit = slot;
//	Bind();
//
//	// Texture stretching
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// Texture filtering(blurred or pixeled)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// zoomed out so far
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// zoomed in so close
//
//
//
//
//	// Checks for errors
//	assert(data && "Failed to create GLFW window");
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, format, pixel_type, data);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	stbi_image_free(data);
//	Unbind();
//}

// Binds texture
void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}
// Unbinds texture
void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
// Deletes texture
void Texture::Delete()
{
	glDeleteTextures(1, &id);
}

Texture::~Texture() {
	if (id != -1) {
		glDeleteTextures(1, &id);
	}
}

Texture::Texture(Texture&& move) noexcept {
	std::swap(id, move.id);
	std::swap(img_width, move.img_width);
	std::swap(img_height, move.img_height);
	std::swap(num_col_chan, move.num_col_chan);
	std::swap(data, move.data);
	std::swap(type, move.type);
	std::swap(path, move.path);
}

Texture& Texture::operator=(Texture&& other) noexcept {
	std::swap(id, other.id);
	std::swap(img_width, other.img_width);
	std::swap(img_height, other.img_height);
	std::swap(num_col_chan, other.num_col_chan);
	std::swap(data, other.data);
	std::swap(type, other.type);
	std::swap(path, other.path);
	return *this;
}