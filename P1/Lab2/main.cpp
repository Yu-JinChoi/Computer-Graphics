#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <crtdbg.h>  

#include "WindowGL.h"
#include "Mesh.h"
#include "Shader.h"

const float toRadian = 3.14159265f / 180.0f;

WindowGL mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

void CreateShaders()
{
	Shader *shader = new Shader();
	//shader->CreateFromString(vShader, fShader);
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(shader);
}

void CreateObject()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	Mesh *obj = new Mesh();
	obj->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);

}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	mainWindow = WindowGL(800, 600);
	mainWindow.Initialize();

	
	CreateObject();

	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
				
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();

		glm::mat4 model = glm::mat4{ 1.0f }; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
				
		glUseProgram(0); //unassign shader

		mainWindow.swapBuffers();
	}
	
	for (auto m : meshList)
	{
		delete m;
	}
	for (auto s : shaderList)
	{
		delete s;
	}

	return 0;
}