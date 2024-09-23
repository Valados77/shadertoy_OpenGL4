#pragma once

#include <glad/glad.h>

class vertexBuffer
{
	private:
		GLuint _handle;

	public:
		vertexBuffer();
		~vertexBuffer();

		vertexBuffer(const vertexBuffer& ) = delete;
		vertexBuffer& operator = (const vertexBuffer& ) = delete;

		void init(const void* data, const GLsizeiptr size);
		void update(const void* data, const GLsizeiptr size) const;
		void bind() const;
		void unbind() const;

		GLint getHandle() const;

};
