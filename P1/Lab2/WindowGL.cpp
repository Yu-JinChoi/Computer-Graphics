#include "WindowGL.h"



WindowGL::WindowGL()
{
	width = 800;
	height = 600;
}

WindowGL::WindowGL(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

int WindowGL::Initialize()
{
	if (!glfwInit())
	{
		std::cout << "GLFW 초기화 실패" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		std::cout << "GLFW 윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW 초기화 실패" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	return 0;
}


WindowGL::~WindowGL()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
