#include "WindowGL.h"



WindowGL::WindowGL()
{
	width = 800;
	height = 600;

	xChange = 0;
	yChange = 0;
}

WindowGL::WindowGL(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	xChange = 0;
	yChange = 0;
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

	//콜백 생성. 윈도우에서 입력이 발생했을시 호출될 함수를 등록
	createCallbacks();
	//현재 윈도우에 대한 포인터 생성
	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

GLfloat WindowGL::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat WindowGL::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}



WindowGL::~WindowGL()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

void WindowGL::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	WindowGL *theWindow = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			std::cout << "Pressed : " << key << std::endl;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			std::cout << "Released : " << key << std::endl;
		}
	}
}

void WindowGL::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	WindowGL *theWindow = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	std::cout << "XChange : " << theWindow->xChange << ", YChange : " << theWindow->yChange << std::endl;
}

void WindowGL::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

