#pragma once

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include<string>
#include <map>


class ShaderLoader
{
private:
	std::string ReadShader(const char *filename);
	GLuint CreateShader(GLenum shaderType, std::string source, const  char* shaderName);

public:
	ShaderLoader(void);
	~ShaderLoader(void);
	GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename, const char* vertexShaderName, const  char* fragmentShaderName);
	GLuint CreateGeoProgram(	const char* vertexShaderFilename, const char* fragmentShaderFilename, const char* geometryShaderFilename,
							const char* vertexShaderName, const char* fragmentShaderName, const char* geometryShaderName);
	GLuint CreateTessProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename, const char* tessControlFilename, const char* tessEvalFilename,
							const char* vertexShaderName, const char* fragmentShaderName, const char* tessControlShaderName, const char* tessEvalShaderName);

private:
	std::map<std::string, GLuint> vertexShaderMap;
	std::map<std::string, GLuint> fragmentShaderMap;
	std::map<std::string, GLuint> geometryShaderMap;
	std::map<std::string, GLuint> tessControlShaderMap;
	std::map<std::string, GLuint> tessEvalShaderMap;
	std::map<std::string, GLuint> programMap;
};
