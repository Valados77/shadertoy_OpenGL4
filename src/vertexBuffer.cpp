#include "vertexBuffer.h"

vertexBuffer::vertexBuffer(): _handle(0) {}

vertexBuffer::~vertexBuffer()
{
	if (_handle == 0) return;
	glDeleteBuffers(1, &_handle);
}

void vertexBuffer::init(const void* data, const GLsizeiptr size)
{
	glGenBuffers(1, &_handle);
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void vertexBuffer::update(const void *data, const GLsizeiptr size) const
{
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void vertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
}

void vertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLint vertexBuffer::getHandle() const
{
	return _handle;
}
