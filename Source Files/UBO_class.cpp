#include"UBO_class.h"

UniformBuffer::UniformBuffer(unsigned int size) {
	glGenBuffers(1, &ubo_id);
	Bind();
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	Unbind();
	glBindBufferRange(GL_UNIFORM_BUFFER, GetId(), ubo_id, 0, size);
}

void UniformBuffer::Bind() {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
}

void UniformBuffer::Unbind() {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//template <typename T> void UniformBuffer::SubData(const unsigned int position, const unsigned int size, T* value) {
//	Bind();
//	glBufferSubData(GL_UNIFORM_BUFFER, position, size, value);
//	Unbind();
//}

unsigned int UniformBuffer::GetId() {
	return ubo_id - 1;
}

UniformBuffer::UniformBuffer(UniformBuffer&& move) noexcept {
	std::swap(ubo_id, move.ubo_id);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {
	std::swap(ubo_id, other.ubo_id);
	return *this;
}

UniformBuffer::~UniformBuffer() {
}