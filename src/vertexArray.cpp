#include "vertexArray.h"

vertexArray::vertexArray(): _handle(0) {}

vertexArray::~vertexArray()
{
	if (_handle == 0) return;
	glDeleteVertexArrays(1, &_handle);
}

void vertexArray::init()
{
	glGenVertexArrays(1, &_handle);
	glBindVertexArray(_handle);
}

void vertexArray::bind()
{
	glBindVertexArray(_handle);
}

void vertexArray::unbind()
{
	glBindVertexArray(0);
}

void vertexArray::bindVertexBuffer(GLuint index, GLuint VB, GLuint blockSize)
{
	glBindVertexBuffer(index, VB, 0, sizeof(GLfloat) * blockSize);
}

void vertexArray::bindVertexAttrib(GLuint Findex, GLuint VBindex, GLuint size, GLint blockSize)
{
	glEnableVertexAttribArray(Findex);
	glVertexAttribFormat(Findex, size, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * blockSize);
	glVertexAttribBinding(Findex, VBindex);
}

