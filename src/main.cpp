#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "GLSLProgram.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"

GLfloat screen[]
{ 
	-1.f, -1.f, 0.f, 0.f, 0.f,
	-1.f,  1.f, 0.f, 0.f, 1.f,
	 1.f,  1.f, 0.f, 1.f, 1.f,
	 1.f, -1.f, 0.f, 1.f, 0.f,
};


GLint screenIndices[]
{
	0, 1, 2,
	0, 2, 3,
};

void error_callback(int, const char*);
static void key_callback(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow*, int, int);
void debugCallback(
		GLenum source,
	   	GLenum type,
	   	GLuint id,
	   	GLenum severity,
	   	GLsizei length,
	   	const GLchar* message,
	   	const void* param);

std::string readFile(const std::string& fname);

int main(void)
{
	glfwSetErrorCallback(error_callback);
	GLFWwindow* window;
	glm::vec2 resolution(1200, 675);
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    window = glfwCreateWindow(resolution.x, resolution.y, "Hello World", NULL, NULL);

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

	glfwMakeContextCurrent(window);
	
	gladLoadGL();

	glDebugMessageCallback(debugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	
	glEnable(GL_DEBUG_OUTPUT);

	vertexArray VAO1;
	VAO1.init();

	vertexBuffer VBO1;
	VBO1.init(screen, sizeof(screen));

	indexBuffer EBO1;
	EBO1.init(screenIndices, sizeof(screenIndices));

	VAO1.bindVertexBuffer(0, VBO1.getHandle(), 5);
	VAO1.bindVertexAttrib(0, 0, 3, 0);
	VAO1.bindVertexAttrib(1, 0, 2, 3);
	{
/////////////////////////////////////////////////////////////////////////////
//	std::string vertCode = readFile("bin/res/shader1.vert");
//	std::string fragCode1 = readFile("bin/res/shader1.frag");
//	std::string fragCode2 = readFile("bin/res/shader2.frag");
//
//	GLuint programs[3];
//	const GLchar* codePtrs[] = {vertCode.c_str(), fragCode1.c_str(), fragCode2.c_str()};
//	programs[0] = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, codePtrs);
//	programs[1] = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, codePtrs + 1);
//	programs[2] = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, codePtrs + 2);
//
//	GLuint pipelines[2];
//	glCreateProgramPipelines(2, pipelines);
//	glUseProgramStages(pipelines[0], GL_VERTEX_SHADER_BIT, programs[0]);
//	glUseProgramStages(pipelines[0], GL_FRAGMENT_SHADER_BIT, programs[1]);
//
//	glUseProgramStages(pipelines[1], GL_VERTEX_SHADER_BIT, programs[0]);
//	glUseProgramStages(pipelines[1], GL_FRAGMENT_SHADER_BIT, programs[2]);
/////////////////////////////////////////////////////////////////////////////
}

	GLSLProgram program;
	program.compileShader("bin/res/shader1.vert");
	program.compileShader("bin/res/shader2.frag");	
	program.link();
	program.validate();
	program.use();
	
/*
	GLuint blockIndex = glGetUniformBlockIndex(program.getHandle(), "Parameters");

	GLint blockSize;
	glGetActiveUniformBlockiv(program.getHandle(), blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte* blockBuffer;
	blockBuffer = (GLubyte*) malloc (blockSize);

	const GLchar* names[] = {"Resolution", "Time", "Test"};
	GLuint indices[3];
	glGetUniformIndices(program.getHandle(), 3, names, indices);
	GLint offset [3];
	glGetActiveUniformsiv(program.getHandle(), 3, indices, GL_UNIFORM_OFFSET, offset);

	GLfloat time = glfwGetTime();
	GLfloat test = 1.0;
	memcpy(blockBuffer + offset[0], &resolution, sizeof(glm::vec2));
	memcpy(blockBuffer + offset[1], &time, sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &test, sizeof(GLfloat));
	
	GLuint UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
*/
	program.setUniform("iTime", (float)glfwGetTime());	
	program.setUniform("iResorlution", resolution);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		VAO1.bind();
		//glBindProgramPipeline(pipelines[0]);
		program.use();
		std::cout << (float)glfwGetTime() << std::endl;
		program.setUniform("iTime", (float)glfwGetTime());	
		

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, sizeof(screenIndices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);


		glEnable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

std::string readFile(const std::string& fname)
{
	std::ifstream inFile(fname);
	if (!inFile) {
		fprintf(stderr, "Error opening file: %s\n", fname.c_str());
		exit(1);
	}

	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	std::string codeStr = code.str();

	return codeStr;
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0,width,height);
}

void debugCallback(
		GLenum source,
	   	GLenum type,
	   	GLuint id,
	   	GLenum severity,
	   	GLsizei length,
	   	const GLchar* message,
	   	const void* param)
{
	const char* sourceStr;
	const char* typeStr;
	const char* severityStr;

	// Определение строки для источника сообщения
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
		default:                              sourceStr = "Unknown"; break;
	}

	// Определение строки для типа сообщения
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
		default:                                typeStr = "Unknown"; break;
	}

	// Определение строки для уровня важности сообщения
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         severityStr = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          severityStr = "Low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; break;
		default:                             severityStr = "Unknown"; break;
	}

	// Вывод сообщения
	printf("Source: %s | Type: %s | Severity: %s | ID: %d | Message: %s\n", 
	       sourceStr, typeStr, severityStr, id, message);
}
