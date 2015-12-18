//Created for Graphics I and II
//Author: Minsi Chen

#include "TriangleMesh.h"
#include "OBJFileReader.h"
#include <Windows.h>
#include "gl/GL.h"

TriangleMesh::TriangleMesh()
{
	m_vertices = NULL;
	m_triangles = NULL;
	m_numTriangles = 0;
	m_numVertices = 0;
}


TriangleMesh::~TriangleMesh()
{
	CleanUp();
}

void TriangleMesh::Render(const Matrix4x4* trans)
{
	//TODO: Basic Feature
	//1. Render the TriangleMesh using the vertices and triangle data stored in m_vertices and m_triangle
	//2. Transform the triangle mesh using the given transformation trans
	

	for (int currentTriangle = 0; currentTriangle < m_numTriangles; ++currentTriangle) {

		int* vertexIndices;	//The indexes of the vertices making up the triangle
		vertexIndices = (int *)m_triangles[currentTriangle].GetVertexIndices();

		Vector4D point1(m_vertices[vertexIndices[0]].m_position[0], 
			m_vertices[vertexIndices[0]].m_position[1],
			m_vertices[vertexIndices[0]].m_position[2]);

		Vector4D point2(m_vertices[vertexIndices[1]].m_position[0],
			m_vertices[vertexIndices[1]].m_position[1],
			m_vertices[vertexIndices[1]].m_position[2]);

		Vector4D point3(m_vertices[vertexIndices[2]].m_position[0],
			m_vertices[vertexIndices[2]].m_position[1],
			m_vertices[vertexIndices[2]].m_position[2]);

		//Normalise the points

		//point1.Normalise();
		//point2.Normalise();
		//point3.Normalise();

		point1 = *trans * point1;
		point2 = *trans * point2;
		point3 = *trans * point3;

		Vector4D side1 = point2 - point1;
		Vector4D side2 = point3 - point1;

		Vector4D lineToCamera = point1 - cameraPosition;
		lineToCamera.Normalise();
		
		Vector4D normal = side1.CrossProduct(side2);
		normal.Normalise();

		//Backface culling currently commented out
		//if (normal.DotProduct(point1) < 0) {
			glNormal3f(normal[0], normal[1], normal[2]);

			glColor3f(0.5f, 0.2f, 0.2f);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBegin(GL_TRIANGLES);
			glVertex3f(point1[0], point1[1], point1[2]);
			glVertex3f(point2[0], point2[1], point2[2]);
			glVertex3f(point3[0], point3[1], point3[2]);
			glEnd();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//}
	}

	//TODO: Advanced Feature
	//Use the OpenGL fixed pipeline to render a lit mesh
	//Note: for per-face lighting you need to compute the normal for each triangle see ComputeTriangleNormals
	// for per-vertex lighting you need to compute the normal for each vertex see ComputeVertexNormals
}

void TriangleMesh::ComputeTriangleNormals()
{
	//TODO: Advanced feature
	//Compute the normal for each triangle stored in m_triangles

	for (int currentTriangle = 0; currentTriangle < m_numTriangles; ++currentTriangle) {

		int* vertexIndices;	//The indexes of the vertices making up the triangle
		vertexIndices = (int *)m_triangles[currentTriangle].GetVertexIndices();

		Vector4D point1(m_vertices[vertexIndices[0]].m_position[0],
			m_vertices[vertexIndices[0]].m_position[1],
			m_vertices[vertexIndices[0]].m_position[2]);

		Vector4D point2(m_vertices[vertexIndices[1]].m_position[0],
			m_vertices[vertexIndices[1]].m_position[1],
			m_vertices[vertexIndices[1]].m_position[2]);

		Vector4D point3(m_vertices[vertexIndices[2]].m_position[0],
			m_vertices[vertexIndices[2]].m_position[1],
			m_vertices[vertexIndices[2]].m_position[2]);

		Vector4D side1 = point2 - point1;
		Vector4D side2 = point3 - point1;

		Vector4D normal = side1.CrossProduct(side2);
		normal.Normalise();

		m_triangles[currentTriangle].SetNormal(normal);
	}
}

void TriangleMesh::ComputeVertexNormals()
{
	//TODO: Advanced feature
	//Compute the normal for each vertex stored in m_vertices
}

bool TriangleMesh::LoadMeshFromOBJFile(const char* filename)
{
	//First pass: reading the OBJ to gether info related to the mesh
	int numVertices = 0;
	int numNormals = 0;
	int numTexCoords = 0;
	int numTriangles = 0;

	if (OBJFileReader::firstPassOBJRead(filename, &numVertices, &numNormals, &numTexCoords, &numTriangles))
	{
		printf("Error parsing file: %s\n", filename);
		return false;
	}
	
	//allocate memory for m_vertices and m_triangles based on the number of vertices and triangles from the first pass
	if (m_vertices && m_triangles)
	{
		printf("Vertex array and triangle array have already been allocated.\n");
		return false;
	}

	m_vertices = new Vertex[numVertices];
	m_triangles = new Triangle[numTriangles];
	
	if (OBJFileReader::secondPassOBJRead(filename, m_vertices, m_triangles))
	{
		printf("Error parsing file: %s\n", filename);
		return false;
	}

	ComputeTriangleNormals();
	ComputeVertexNormals();

	m_numTriangles = numTriangles;
	m_numVertices = numVertices;
	
	return true;
}

void TriangleMesh::CleanUp()
{
	if (m_vertices)
		delete[] m_vertices;
	if (m_triangles)
		delete[] m_triangles;

	m_vertices = NULL;
	m_triangles = NULL;
	m_numTriangles = 0;
	m_numVertices = 0;
}

void TriangleMesh::UpdateCameraPosition(Vector4D* cameraPosition) {
	this->cameraPosition = *cameraPosition;
}