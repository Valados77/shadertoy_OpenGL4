#pragma once

#include <glad/glad.h>

class vertexArray
{
	private:
		GLuint _handle;

	public:
		vertexArray();
		~vertexArray();

		vertexArray(const vertexArray& ) = delete;
		vertexArray& operator = (const vertexArray& ) = delete;

		void init();

		void bind(); 
		void unbind();

		void bindVertexBuffer(GLuint index, GLuint VB, GLuint blockSize);
		void bindVertexAttrib(GLuint Findex, GLuint VBindex, GLuint size, GLint blockSize);

};
