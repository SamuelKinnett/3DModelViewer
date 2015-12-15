//Created for Graphics I and II
//Author: Minsi Chen
#include "OGLCube.h"

#include <windows.h>
#include <gl/GL.h>

OGLCube::OGLCube()
{
	InitUnitCube();
}

OGLCube::~OGLCube()
{
	delete [] m_vertices;
	delete [] m_indices;
}

void OGLCube::InitUnitCube()
{
	//Define a list of vertices , i.e. cube corners
	SimpleVertex corners[] = 
	{
		{Vector4D(-0.5f, -0.5f, 0.5f), Vector4D(0.0f, 0.0f, 0.0f)},// v0
		{Vector4D(0.5f, -0.5f, 0.5f), Vector4D(1.0f, 0.0f, 0.0f)}, // v1
		{Vector4D(0.5f, 0.5f, 0.5f), Vector4D(1.0f, 1.0f, 0.0f)}, // v2
		{Vector4D(-0.5f, 0.5f, 0.5f), Vector4D(0.0f, 1.0f, 0.0f)}, // v3

		{Vector4D(-0.5f, -0.5f, -0.5f), Vector4D(0.0f, 0.0f, 1.0f)}, // v4
		{Vector4D(0.5f, -0.5f, -0.5f), Vector4D(1.0f, 0.0f, 1.0f)}, // v5
		{Vector4D(0.5f, 0.5f, -0.5f), Vector4D(1.0f, 1.0f, 1.0f)}, // v6
		{Vector4D(-0.5f, 0.5f, -0.5f), Vector4D(0.0f, 1.0f, 1.0f)}, // v7
	};

	//Use triangles to define the faces, i.e. 2 triangles for one cube face
	unsigned int indices[] = 
	{
		0, 1, 2, //+z face
		0, 2, 3,

		1, 5, 6, //+x face
		1, 6, 2,

		4, 7, 6, //-z face
		4, 6, 5,

		0, 3, 7, //-x face
		0, 7, 4,

		2, 6, 7, //+y face
		2, 7, 3,

		0, 4, 5, //-y face
		0, 5, 1
	};

	m_vertices = new SimpleVertex[8];
	m_indices = new unsigned int[36];

	memcpy_s(m_vertices, sizeof(SimpleVertex) * 8, corners, sizeof(SimpleVertex) * 8);
	memcpy_s(m_indices, sizeof(unsigned int) *36,  indices, sizeof(unsigned int) * 36);
}

void OGLCube::Render(const Matrix4x4* trans)
{
	//TODO: 
	// 1. Draw the cube as a list of triangles (e.g. GL_TRIANGLES) stored in m_indices.
	// Each index in m_indices refers to a vertex stored in m_vertices.
	// Every 3 consecutive indices in m_indices define the three vertices of a triangle
	//
	// 2. Use the transformation matrix trans to transform the position of all vertices
	// so that the rendered cube is transformed by the matrix

	for (int currentTriangle = 0; currentTriangle < 36; currentTriangle += 3) {
		
		Vector4D point1(m_vertices[m_indices[currentTriangle]].position[0],
			m_vertices[m_indices[currentTriangle]].position[1],
			m_vertices[m_indices[currentTriangle]].position[2]);

		Vector4D point2(m_vertices[m_indices[currentTriangle + 1]].position[0],
			m_vertices[m_indices[currentTriangle + 1]].position[1],
			m_vertices[m_indices[currentTriangle + 1]].position[2]);

		Vector4D point3(m_vertices[m_indices[currentTriangle + 2]].position[0],
			m_vertices[m_indices[currentTriangle + 2]].position[1],
			m_vertices[m_indices[currentTriangle + 2]].position[2]);

		point1 = *trans * point1;
		point2 = *trans * point2;
		point3 = *trans * point3;

		glColor3ub(255, 255, 255);
		glBegin(GL_LINE_LOOP);
		glVertex3f(point1[0], point1[1], point1[2]);
		glVertex3f(point2[0], point2[1], point2[2]);
		glVertex3f(point3[0], point3[1], point3[2]);
		glEnd();
	}

}
