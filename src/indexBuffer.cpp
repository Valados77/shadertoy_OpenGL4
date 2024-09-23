#include "indexBuffer.h"

indexBuffer::indexBuffer(): _handle(0) {}

indexBuffer::~indexBuffer()
{
	if (_handle == 0) return;
	glDeleteBuffers(1, &_handle);
}

void indexBuffer::init(const void* data, const GLsizeiptr size)
{
	glGenBuffers(1, &_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void indexBuffer::update(const void *data, const GLsizeiptr size) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
}

void indexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
}

void indexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLint indexBuffer::getHandle() const
{
	return _handle;
}
