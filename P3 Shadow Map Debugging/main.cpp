#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>

#include <crtdbg.h>  

#include "CommonValues.h"
#include "WindowGL.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

const float toRadian = 3.14159265f / 180.0f;

GLuint uniformProjection = 0;
GLuint uniformModel = 0;
GLuint uniformView = 0;
GLuint uniformEyePosition = 0;
GLuint uniformSpecularIntensity = 0;
GLuint uniformShininess = 0;

WindowGL mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Shader directionalShadowShader;
Shader depthmapDebugShader;

Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

Model xwing;
Model blackhawk;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

void CreateShaders()
{
	Shader *shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(shader);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");

	//depth texture debug shader
	depthmapDebugShader = Shader();
	depthmapDebugShader.CreateFromFiles("Shaders/depthmap_debug.vert", "Shaders/depthmap_debug.frag");
}

void calcAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat* vertices,
	unsigned int verticesCount, unsigned int vLength, unsigned int normalOffset)
{
	//vLength : vertex 하나당 관련되어 있는 데이터의 갯수
	//normalOffset : vertex에서 normal 데이터가 시작하는 위치 offset

	//Process 
	// 1) (첫번째 for loop) triangle별로 cross product를 통해 normal을 계산
	// 2) (첫번째 for loop) 각 vertex의 normal에 계산된 normal을 더함
	// 3) (두번째 for loop) 더해진 normal을 normalize

	for (size_t i{ 0 }; i < indicesCount; i += 3)
	{
		unsigned int in0 = indices[i]     * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		//in0를 기준으로 하는 두 벡터
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
	
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i{ 0 }; i < verticesCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObject()
{
	GLfloat vertices[] = {
		// x ,   y  ,  z  ,     u  ,  v  ,   nx ,  ny ,  nz
		-1.0f, -1.0f, -0.6f,    0.0f, 0.0f,	0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,    0.5f, 0.0f,	0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,    1.0f, 0.0f,	0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,    0.5f, 1.0f,	0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,  1.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,  0.0f,  1.0f, 0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,  1.0f,  1.0f, 0.0f, -1.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj = new Mesh();
	obj->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	//2nd floor for depth texture debug
	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj4);

}

void RenderScene() //render pass 분리
{
	glm::mat4 model = glm::mat4{ 1.0f };
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	model = glm::mat4{ 1.0f };
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	dirtTexture.UseTexture();
	meshList[1]->RenderMesh();

	//Floor
	model = glm::mat4{ 1.0f };
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	dirtTexture.UseTexture();
	meshList[2]->RenderMesh();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShaodwMap()->GetShadowWidth(), light->GetShaodwMap()->GetShadowHeight()); //프레임 버퍼와 동일한 크기로

	light->GetShaodwMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation(); //값의 설정은 render pass에서 수행함
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	shaderList[0]->UseShader();

	uniformModel = shaderList[0]->GetModelLocation();
	uniformProjection = shaderList[0]->GetProjectionLocation();
	uniformView = shaderList[0]->GetViewLocation();

	uniformEyePosition = shaderList[0]->GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();
	uniformShininess = shaderList[0]->GetShininessLocation();

	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//두 모델에서 공통적인 값
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
		camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0]->SetDirectionalLight(&mainLight);
	shaderList[0]->SetPointLights(pointLights, pointLightCount);
	shaderList[0]->SetSpotLights(spotLights, spotLightCount);
	shaderList[0]->SetDirectionalLightTransform(&mainLight.CalculateLightTransform()); //shadow 적용을 위해 light transform 전달
	
	mainLight.GetShaodwMap()->Read(GL_TEXTURE1); //texture unit을 activate하고 shadowmap에 bind, Texture0는 theTexture(모델의 텍스처)에 사용됨
	shaderList[0]->SetTexture(0);
	shaderList[0]->SetDirectionalShaodwMap(1);
	
	RenderScene();
}

void RenderDepthMap(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	//--------Depth map을 보이기 위해 만든 2nd floor(meshList[3])를 렌더링하는 코드를 작성하시오---------//
	//1) 2nd floor또한 world 공간 내에 위치하므로, model, view, projection matrix정보가 vertex shader로 전달되어야 함
	//2) 1st pass에서  렌더링된 shadow map이 fragment shader로 전달되어야 함
	//3) 2nd floor를 적절한 곳에 위치시키고 렌더링을 수행해야 함 (아래 작성되어 있음)

	depthmapDebugShader.UseShader();

	uniformModel = depthmapDebugShader.GetModelLocation();
	uniformProjection = depthmapDebugShader.GetProjectionLocation();
	uniformView = depthmapDebugShader.GetViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	depthmapDebugShader.SetDirectionalLight(&mainLight);
	depthmapDebugShader.SetPointLights(pointLights, pointLightCount);
	depthmapDebugShader.SetSpotLights(spotLights, spotLightCount);
	depthmapDebugShader.SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.GetShaodwMap()->Read(GL_TEXTURE1);
	depthmapDebugShader.SetTexture(0);
	depthmapDebugShader.SetDirectionalShaodwMap(1);

	glm::mat4 model = glm::mat4{ 1.0f };
	model = glm::translate(model, glm::vec3(25.0f, -2.0f, 0.0f));
	model = glm::rotate(model, 180* toRadian, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	meshList[3]->RenderMesh();
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	mainWindow = WindowGL(800, 600);
	mainWindow.Initialize();

	camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);
		
	CreateObject();

	CreateShaders();

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();


	shinyMaterial = Material(4.0f, 256.0f);
	dullMaterial = Material(0.3f, 4.0f);

	glm::vec3 mainLightDir = glm::vec3(0.0f, -7.0f, -1.0f);
	mainLight = DirectionalLight(256,256, //shadow buffer 크기
								1.0f, 1.0f, 1.0f, 
								0.1f, 0.6f,
								mainLightDir.x, mainLightDir.y, mainLightDir.z);
								//0.0f, -15.0f, -10.0f);
	
	brickTexture.UseTexture();
	dirtTexture.UseTexture();

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

	Assimp::Importer impoerter = Assimp::Importer();

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		if (mainWindow.getKeys()[GLFW_KEY_RIGHT])
		{
			mainLightDir.x += 0.1f;
			mainLight.SetDirection(mainLightDir.x, mainLightDir.y, mainLightDir.z);
		}
		if (mainWindow.getKeys()[GLFW_KEY_LEFT])
		{
			mainLightDir.x -= 0.1f;
			mainLight.SetDirection(mainLightDir.x, mainLightDir.y, mainLightDir.z);
		}
		if (mainWindow.getKeys()[GLFW_KEY_UP])
		{
			mainLightDir.z -= 0.1f;
			mainLight.SetDirection(mainLightDir.x, mainLightDir.y, mainLightDir.z);
		}
		if (mainWindow.getKeys()[GLFW_KEY_DOWN])
		{
			mainLightDir.z += 0.1f;
			mainLight.SetDirection(mainLightDir.x, mainLightDir.y, mainLightDir.z);
		}

		camera.keyControl(mainWindow.getKeys(),deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		DirectionalShadowMapPass(&mainLight);
		RenderPass(projection, camera.calculateViewMatrix());
		RenderDepthMap(projection, camera.calculateViewMatrix());
						
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