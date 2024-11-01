#include "Texture_class.h"

Texture::Texture(const char* image, GLenum tex_wrapping, GLenum format, bool flip) {
	// Generates textures names, activates and binds textures
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();

	// Texture stretching
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_wrapping);

	// Texture filtering(blurred or pixeled)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// zoomed out so far
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// zoomed in so close

	// Texture flip
	stbi_set_flip_vertically_on_load(flip);
	// Gets data about texture from file
	data = stbi_load(image, &img_width, &img_height, &num_col_chan, 0);
	// Checks for errors
	assert(data && "Failed to load texture");
	// Generates currently bounded texture
	glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, data);
	// Generates mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// Frees data
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);
}

Texture::Texture(const char* image, GLenum tex_wrapping, GLenum format, GLenum internal_format, bool flip) {
	// Generates textures names, activates and binds textures
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();

	// Texture stretching
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_wrapping);

	// Texture filtering(blurred or pixeled)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// zoomed out so far
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// zoomed in so close

	// Texture flip
	stbi_set_flip_vertically_on_load(flip);
	// Gets data about texture from file
	data = stbi_load(image, &img_width, &img_height, &num_col_chan, 0);
	// Checks for errors
	assert(data && "Failed to load texture");
	// Generates currently bounded texture
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, data);
	// Generates mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// Frees data
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);
}

Texture::Texture(GLenum internal_format, const unsigned int tex_width, const unsigned int tex_height, GLenum format, GLenum type, std::vector<glm::vec3>& ssao_noise) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, tex_width, tex_height, 0, format, type, &ssao_noise[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::Texture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D, id, 0);
}

Texture::Texture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget, GLenum internal_format) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img_width, img_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D, id, 0);
}

ShadowTexture::ShadowTexture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D, id, 0);
}

ShadowTexture::ShadowTexture(GLenum format, const unsigned int img_width, const unsigned int img_height, GLenum type, GLenum textarget, GLenum internal_format) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img_width, img_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D, id, 0);
}



Texture::Texture(const unsigned int shadow_width, const unsigned int shadow_height) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
}


Texture::Texture(const int rows, const int cols, const char* faces) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < rows; i++)
	{
		data = stbi_load((char*)(faces + i * (cols)), &width, &height, &nrChannels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			printf("Cubemap texture failed to load at path: %s\n", ((char*)(faces + i * (cols))));
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
}

CubemapShadowTexture::CubemapShadowTexture(GLenum format, unsigned int width, unsigned int height,GLenum type, GLenum textarget) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (unsigned int i = 0; i < 6; i++) 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, type, NULL);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, textarget, id, 0);
}

MultisampledTexture::MultisampledTexture(GLenum format, const unsigned int samples_amount, const unsigned int img_width, const unsigned int img_height, GLenum textarget) {
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	BindMultisample();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_amount, format, img_width, img_height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, textarget, GL_TEXTURE_2D_MULTISAMPLE, id, 0);
}

// Models textures
Texture::Texture(std::string filename, const std::string directory, const char* typeName, bool gamma) {
	this->type = typeName;
	this->filename = filename;
	// name of the file
	std::string path = std::string(filename);
	// path to the file
	path = directory + "/textures/" + path;
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + id - 1);
	int width, height, nrComponents;
	// gets data about texture from file
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		// chooses format
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		// binds texture
		glBindTexture(GL_TEXTURE_2D, id);
		// generates currently bounded texture
 		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// generates mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// texture stretching
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// texture filtering(blurred or pixeled)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// frees data
		stbi_image_free(data);
	}
	else
	{
		printf("Texture failed to load at path: %s\n", filename);
		// frees data
		stbi_image_free(data);
	}
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

void Texture::BindMultisample() {
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
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


void Texture::SetId(GLuint id) {
	this->id = id;
}

GLuint Texture::GetId() {
	return id - 1;
}


void Texture::SetType(std::string type) {
	this->type = type;
}

std::string Texture::GetType() {
	return type;
}

void Texture::SetFilename(std::string filename) {
	this->filename = filename;
}

std::string Texture::GetFilename() {
	return filename;
}


Texture::~Texture() {
	if (id != -1)
		glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& move) noexcept {
	std::swap(id, move.id);
	std::swap(img_width, move.img_width);
	std::swap(img_height, move.img_height);
	std::swap(num_col_chan, move.num_col_chan);
	std::swap(data, move.data);
	std::swap(type, move.type);
	std::swap(filename, move.filename);
}

Texture& Texture::operator=(Texture&& other) noexcept {
	std::swap(id, other.id);
	std::swap(img_width, other.img_width);
	std::swap(img_height, other.img_height);
	std::swap(num_col_chan, other.num_col_chan);
	std::swap(data, other.data);
	std::swap(type, other.type);
	std::swap(filename, other.filename);
	return *this;
}