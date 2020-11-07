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

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~WindowGL();

private:
	GLFWwindow *mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

