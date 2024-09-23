#pragma once

#include <glad/glad.h>

class indexBuffer
{
	private:
		GLuint _handle;

	public:
		indexBuffer();
		~indexBuffer();

		indexBuffer(const indexBuffer& ) = delete;
		indexBuffer& operator = (const indexBuffer& ) = delete;

		void init(const void* data, const GLsizeiptr size);
		void update(const void* data, const GLsizeiptr size) const;
		void bind() const;
		void unbind() const;

		GLint getHandle() const;

};
