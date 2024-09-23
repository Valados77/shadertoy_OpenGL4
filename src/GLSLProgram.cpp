#include "GLSLProgram.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace GLSLShaderInfo
{
	std::map<std::string, GLSLShader::GLSLShaderType> extensions = {
		{".vs",   GLSLShader::VERTEX},
		{".vert", GLSLShader::VERTEX},
		{"_vert.glsl", GLSLShader::VERTEX},
		{".vert.glsl", GLSLShader::VERTEX },
		{".gs",   GLSLShader::GEOMETRY},
		{".geom", GLSLShader::GEOMETRY},
		{ ".geom.glsl", GLSLShader::GEOMETRY },
		{".tcs",  GLSLShader::TESS_CONTROL},
		{ ".tcs.glsl",  GLSLShader::TESS_CONTROL },
		{ ".tes",  GLSLShader::TESS_EVALUATION },
		{".tes.glsl",  GLSLShader::TESS_EVALUATION},
		{".fs",   GLSLShader::FRAGMENT},
		{".frag", GLSLShader::FRAGMENT},
		{"_frag.glsl", GLSLShader::FRAGMENT},
		{".frag.glsl", GLSLShader::FRAGMENT},
		{".cs",   GLSLShader::COMPUTE},
		{ ".cs.glsl",   GLSLShader::COMPUTE }
	};
}

GLSLProgram::GLSLProgram(): handle(0), linked(false) {}

GLSLProgram::~GLSLProgram()
{
	if (handle == 0) return;
	detachAndDeleteShaderObjects();
	glDeleteProgram(handle);
}

void GLSLProgram::detachAndDeleteShaderObjects() 
{
	// Detach and delete the shader objects (if they are not already removed)
	GLint numShaders = 0;
	glGetProgramiv(handle, GL_ATTACHED_SHADERS, &numShaders);
	std::vector<GLuint> shaderNames(numShaders);
	glGetAttachedShaders(handle, numShaders, NULL, shaderNames.data());
	for (GLuint shader : shaderNames) {
		glDetachShader(handle, shader);
		glDeleteShader(shader);
	}
}

void GLSLProgram::compileShader(const char* fileName)
{
	std::string ext = getExtension(fileName);
	GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
	auto it = GLSLShaderInfo::extensions.find(ext);
	if(it != GLSLShaderInfo::extensions.end()) 
	{
		type = it->second;
	}
	else
	{
		std::string msg = "Unrecogized extension: " + ext;
		throw GLSLProgramException(msg);
	}
	compileShader(fileName, type);
}

std::string GLSLProgram::getExtension(const char *name) 
{
	std::string nameStr(name);

    size_t dotLoc = nameStr.find_last_of('.');
    if (dotLoc != std::string::npos) {
        std::string ext = nameStr.substr(dotLoc);
        if( ext == ".glsl" ) {

            size_t loc = nameStr.find_last_of('.', dotLoc - 1);
            if( loc == std::string::npos ) {
                loc = nameStr.find_last_of('_', dotLoc - 1);
            }
            if( loc != std::string::npos ) {
                return nameStr.substr(loc);
            }
        } else {
            return ext;
        }
    }
    return "";
}

void GLSLProgram::compileShader(const char* fileName, GLSLShader::GLSLShaderType type)
{
		if (!fileExists(fileName)) 
		{
			std::string message = std::string("Shader: ") + fileName + " not found.";
			throw GLSLProgramException(message);
		}

		if (handle <= 0) 
		{
			handle = glCreateProgram();
			if (handle == 0) 
			{
				throw GLSLProgramException("Unable to create shader program.");
			}
		}

		std::ifstream inFile(fileName, std::ios::in);
		if (!inFile) {
			std::string message = std::string("Unable to open: ") + fileName;
			throw GLSLProgramException(message);
		}

		// Get file contents
		std::stringstream code;
		code << inFile.rdbuf();
		inFile.close();

		compileShader(code.str(), type, fileName);
}

void GLSLProgram::compileShader(const std::string& source, GLSLShader::GLSLShaderType type, const char* fileName)
{
	if (handle <= 0) 
	{
        handle = glCreateProgram();
        if (handle == 0) 
		{
            throw GLSLProgramException("Unable to create shader program.");
        }
    }

    GLuint shaderHandle = glCreateShader(type);

    const char *c_code = source.c_str();
    glShaderSource(shaderHandle, 1, &c_code, NULL);

    // Compile the shader
    glCompileShader(shaderHandle);

    // Check for errors
    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) 
	{
        // Compile failed, get log
		std::string msg;
		if (fileName) 
		{
			msg = std::string(fileName) + ": shader compliation failed\n";
		}
		else 
		{
			msg = "Shader compilation failed.\n";
		}

        int length = 0;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) 
		{
            std::string log(length, ' ');
            int written = 0;
            glGetShaderInfoLog(shaderHandle, length, &written, &log[0]);
			msg += log;
        }
        throw GLSLProgramException(msg);
    }
	else 
	{
        // Compile succeeded, attach shader
        glAttachShader(handle, shaderHandle);
    }
}

void GLSLProgram::link()
{
	if (linked) 
		return;
    if (handle <= 0) 
		throw GLSLProgramException("Program has not been compiled.");

    glLinkProgram(handle);
	int status = 0;
	std::string errString;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) 
	{
		// Store log and return false
		int length = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
		errString += "Program link failed:\n";
		if (length > 0) 
		{
			std::string log(length,' ');
			int written = 0;
			glGetProgramInfoLog(handle, length, &written, &log[0]);
			errString += log;
		}
	}
	else 
	{
		findUniformLocations();
		linked = true;
	}
	 
	detachAndDeleteShaderObjects();

	if( GL_FALSE == status ) 
		throw GLSLProgramException(errString);
}

void GLSLProgram::use()
{
	if (handle <= 0 || (!linked))
			throw GLSLProgramException("Shader has not been linked");
	glUseProgram(handle);
}

void GLSLProgram::validate()
{
	 if (!isLinked())
        throw GLSLProgramException("Program is not linked");

    GLint status;
    glValidateProgram(handle);
    glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

    if (GL_FALSE == status) 
	{
        // Store log and return false
        int length = 0;
		std::string logString;

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) 
		{
            char *c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }

        throw GLSLProgramException(std::string("Program failed to validate\n") + logString);
    }
}

int GLSLProgram::getHandle()
{
	return handle;
}

bool GLSLProgram::isLinked()
{
	return linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char* name)
{
	glBindAttribLocation(handle, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char* name)
{
	glBindFragDataLocation(handle, location, name);
}

void GLSLProgram::setUniform(const char* name, float x, float y, float z)
{
	GLint loc = getUniformLocation(name);
	glUniform3f(loc, x, y, z);
}

void GLSLProgram::setUniform(const char* name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

void GLSLProgram::setUniform(const char* name, const glm::vec3& v)
{
	this->setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char* name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void GLSLProgram::setUniform(const char* name, const glm::mat3& m)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char* name, const glm::mat4& m)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char* name, float val)
{
	GLint loc = getUniformLocation(name);
    glUniform1f(loc, val);
}

void GLSLProgram::setUniform(const char* name, double val)
{
	GLint loc = getUniformLocation(name);
    glUniform1d(loc, val);
}

void GLSLProgram::setUniform(const char* name, int val)
{
	GLint loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void GLSLProgram::setUniform(const char* name, bool val)
{
	GLint loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void GLSLProgram::findUniformLocations()
{
	uniformLocations.clear();

	GLint numUniforms = 0;
	glGetProgramInterfaceiv( handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};

    for( GLint i = 0; i < numUniforms; ++i ) 
	{
		GLint results[4];
		glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

      	if( results[3] != -1 ) 
			continue;  // Skip uniforms in blocks
      	GLint nameBufSize = results[0] + 1;
      	char * name = new char[nameBufSize];
      	glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name);
      	uniformLocations[name] = results[2];
      	delete [] name;
	}
}

bool GLSLProgram::fileExists(const std::string &fileName) 
{
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}

int GLSLProgram::getUniformLocation(const char *name) 
{
	auto pos = uniformLocations.find(name);

	if (pos == uniformLocations.end()) {
		GLint loc = glGetUniformLocation(handle, name);
		uniformLocations[name] = loc;
		return loc;
	}

	return pos->second;
}
