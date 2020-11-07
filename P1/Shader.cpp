#include "Shader.h"



Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

void Shader::CreateFromFiles(const char * vertexLocation, const char * fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);

}

void Shader::CreateFromString(const char * vertexCode, const char * fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char * fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cout << "File read failed : " << fileLocation << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

GLuint Shader::GetViewLocation()
{
	return uniformView;
}


void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

Shader::~Shader()
{
	ClearShader();
}

void Shader::CompileShader(const char * vertexCode, const char * fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID)
	{
		std::cout << "셰이더 프로그램 생성 실패" << std::endl;
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	//----------------셰이더 링크 에러 처리------------------------------
	{
		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glLinkProgram(shaderID);
		glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
		if (!result)
		{
			glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
			std::cout << "셰이더 프로그램 링크 에러 : " << eLog << std::endl;
			return;
		}

		glValidateProgram(shaderID);
		glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
		if (!result)
		{
			glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
			std::cout << "셰이더 프로그램 검증 에러 : " << eLog << std::endl;
			return;
		}
	}
	//-------------------------------------------------------------------

	//셰이더 내 xMove의 ID를 uniformXMove에 저장
	uniformModel = glGetUniformLocation(shaderID, "model");
	
	uniformProjection = glGetUniformLocation(shaderID, "projection");

	uniformView = glGetUniformLocation(shaderID, "view");
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	//----------------셰이더 컴파일 에러 처리------------------------------
	{
		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
			std::cout << "셰이더 프로그램 컴파일 에러 : (" << shaderType << ") " << eLog << std::endl;
			return;
		}
	}
	//-------------------------------------------------------------------

	glAttachShader(theProgram, theShader);

	return;
}