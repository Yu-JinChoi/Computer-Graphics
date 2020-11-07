#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
class WindowGL
{
public:
	WindowGL();
	WindowGL(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~WindowGL();

private:
	GLFWwindow *mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	//키보드 입력을 저장할 변수
	bool keys[1024];

	//이전 프레임에서 현재 프레임까지 마우스 위치 변화량 계산
	GLfloat lastX, lastY; //커서의 화면상 X,Y 좌표
	GLfloat xChange, yChange; //그 변화량
	bool mouseFirstMoved;

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

	void createCallbacks();
};

