#include "Mesh.h"



Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat * vertices, unsigned int * indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO); //Vertex Array Object(VAO) ID 정의
	glBindVertexArray(VAO); //VAO를 Bind --> 지금부터 VAO 관련 데이터를 정의하겠다는 의미

	glGenBuffers(1, &IBO); //Index Buffer Object(IBO) ID 정의
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); //IBO를 Bind 하면서, Index Array라고 명시
	//실제 데이터 할당
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW); 

	glGenBuffers(1, &VBO); //Vertex Buffer Object(VBO) ID 정의
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO를 Bind 하면서, Vertex Array라고 명시
	//실제 데이터 할당
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*numOfVertices, vertices, GL_STATIC_DRAW); 

	glEnableVertexAttribArray(0); //Attribute Enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //첫 인자가 layout location 정의. 5번째 0은 stride, 마지막 0은 offset
	
	glBindVertexArray(0); //Unbind VAO

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind IBO
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO); //실제로 Buffer data를 해제하여 GPU 메모리 공간 확보
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}


Mesh::~Mesh()
{
	ClearMesh();
}
