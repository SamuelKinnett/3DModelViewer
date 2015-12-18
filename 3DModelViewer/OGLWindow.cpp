#include "Resource.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "OGLWindow.h"
#include "Matrix4x4.h"
#include "Vector4D.h"
#include <cmath>
#include <io.h>
#include <iostream>
#include <string>

OGLWindow::OGLWindow()
{
	
}

OGLWindow::~OGLWindow()
{
	//Clean up the renderable
	delete m_cube;
	models.clear();
	sceneModels.clear();

}

OGLWindow::OGLWindow(HINSTANCE hInstance, int width, int height)
{
	//InitWindow(hInstance, width, height);
}

HGLRC OGLWindow::CreateOGLContext(HDC hdc)
{
	unsigned int pixelformat;
	HGLRC hglrc;

	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Support Window
		PFD_SUPPORT_OPENGL |						// Format Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request RGBA Format
		32,											// Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits mask
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// Z-Buffer depth
		0,											// Stencil Buffer
		0,											// Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(pixelformat=ChoosePixelFormat(hdc,&pfd)))
	{
		return 0;
	}

	if(!SetPixelFormat(hdc,pixelformat,&pfd))
	{
		return 0;
	}

	if (!(hglrc=wglCreateContext(hdc)))
	{
		return 0;
	}

	if(!wglMakeCurrent(hdc,hglrc))
	{
		return 0;
	}

	return hglrc;
}

void OGLWindow::DestroyOGLWindow()
{
	DestroyOGLContext();

	DestroyWindow( m_hwnd );

	m_hwnd = NULL;
	m_hdc = NULL;
}

BOOL OGLWindow::DestroyOGLContext()
{
	if ( m_hglrc )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_hglrc );
		m_hglrc = NULL;
	}
	
	if ( !ReleaseDC ( m_hwnd, m_hdc ) )
		return FALSE;

	return TRUE;
}

BOOL OGLWindow::InitWindow(HINSTANCE hInstance, int width, int height)
{
	RECT clientrect;

	m_hwnd = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		L"OGLWindow", L"OGLWindow", WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		0, 0, width, height, NULL, NULL, hInstance, NULL);

	if ( ! m_hwnd )
		return FALSE;

	m_hdc = GetDC( m_hwnd );

	GetClientRect(m_hwnd, &clientrect);

	SetWindowPos(m_hwnd, NULL, 10, 10, (width << 1) - clientrect.right, (height << 1) - clientrect.bottom, SWP_HIDEWINDOW | SWP_NOREDRAW);
		
	m_width = width;
	m_height = height;

	if (!(m_hglrc = CreateOGLContext(m_hdc)))
		return FALSE;

	//Initialise the OpenGL variables
	InitOGLState();

	//Load the model files
	LoadModels();

	//Create the camera
	m_camera = Camera(*new Vector4D(0, 0, -20),
		*new Vector4D(0, 1, 0),
		*new Vector4D(0, 0, 0)
		);

	//Add a teapot to the world
	sceneModels.push_back(new Model(models[0], &m_camera, 0));
	sceneModels[0]->SetPosition(0, 0, 0);
	sceneModels[0]->SetScale(0.05, 0.05, 0.05);

	return TRUE;
}

void OGLWindow::SetVisible ( BOOL visible )
{
	ShowWindow ( m_hwnd, visible? SW_SHOW : SW_HIDE );
}

void OGLWindow::Render()
{
	++time;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	
	Matrix4x4 transformation;
	Matrix4x4 temporaryTransformation;	//Stores a transformation before it is concatenated
										//with the main transformation.

	//Update the transformation matrices
	UpdateTransformationMatrices();

	//Update the camera position stored in each model
	for (int currentModel = 0; currentModel < sceneModels.size(); ++currentModel) {
		if (currentModel == focusedModel) {
			float* rotation = sceneModels[currentModel]->GetRotation();
			sceneModels[currentModel]->SetRotation(rotation[0] + rotateX,
				rotation[1] + rotateY,
				rotation[2] + rotateZ);
			float* scale = sceneModels[currentModel]->GetScale();
			sceneModels[currentModel]->SetScale(scale[0] + zoom,
				scale[1] + zoom,
				scale[2] + zoom);
		}
		sceneModels[currentModel]->Render();
	}

	//TODO:
	//1. Set the matrix 'transformation' as a rotation about the Z axis
	//
	//2. Set the matrix 'transformation' as a more complex transformation by concatenating, e.g. Rz Rx Ry

	//Set the transformation matrix as a 30 degree rotation about the z axis
	
	//Remember, M.T.R.S.

	glFlush();

	SwapBuffers(m_hdc);
	return;
}

void OGLWindow::Resize( int width, int height )
{
	RECT clientrect;

	GetClientRect(m_hwnd, &clientrect);

	int left = width - clientrect.right;
	int top = height - clientrect.bottom;

	m_width = width - left;
	m_height = height - top;

	double aspectratio = (double)m_width / (double)m_height;

	glViewport(left >> 1, top >>1 , m_width, m_height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	//TODO:
	// Change from orthographic project to perspective projection
	// use either glFrustum or gluPerspective

	//glFrustum(aspectratio * -2.0, aspectratio * 2.0, -2.0, 2.0, 1.0f, 100);
	gluPerspective(90, aspectratio, 1.0f, 100.0f);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	return;
}

void OGLWindow::InitOGLState()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//glClearDepth(0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);

	//Enable lighting
	float diffuseLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float specularLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightPosition[] = { -5.0f, 2.0f, 0.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColorMaterial(GL_FRONT, GL_SPECULAR);
	glShadeModel(GL_FLAT);

}

BOOL OGLWindow::MouseLBDown ( int x, int y )
{

	return TRUE;
}

BOOL OGLWindow::MouseLBUp ( int x, int y )
{
	return TRUE;
}

BOOL OGLWindow::MouseMove ( int x, int y )
{
	return TRUE;
}

BOOL OGLWindow::KeyDown(WPARAM key) {

	if (!keyDown) {
		switch (key) {

		case 0x57:
			//W key
			rotateX = -0.5f;
			break;

		case 0x53:
			//S key
			rotateX = 0.5f;
			break;

		case 0x41:
			//A key
			rotateY = -0.5f;
			break;

		case 0x44:
			//D key
			rotateY = 0.5f;
			break;

		case 0x51:
			//Q key
			rotateZ = -0.5f;
			break;

		case 0x45:
			//E key
			rotateZ = 0.5f;
			break;

		case 0x52:
			//R key
			zoom = -0.0001f;
			break;

		case 0x46:
			//R key
			zoom = 0.0001f;
			break;

		case VK_SPACE:
			//Spacebar
			//Reset rotation
			sceneModels[focusedModel]->SetPosition(0, 0, 0);
			sceneModels[focusedModel]->SetRotation(0, 0, 0);
			sceneModels[focusedModel]->SetScale(1, 1, 1);
			break;


		case VK_PRIOR:
			//Page up, load the previous model in the list
			if (sceneModels[focusedModel]->modelID > 0) {
				--sceneModels[focusedModel]->modelID;
				sceneModels[focusedModel]->SetModel(models[sceneModels[focusedModel]->modelID]);
			}
			break;

		case VK_NEXT:
			//Page down, load the next model in the list
			if (sceneModels[focusedModel]->modelID < totalModels - 1) {
				++sceneModels[focusedModel]->modelID;
				sceneModels[focusedModel]->SetModel(models[sceneModels[focusedModel]->modelID]);
			}
			break;

		}
		lastKey = key;
		keyDown = true;
	}

	return TRUE;
}

BOOL OGLWindow::KeyUp(WPARAM key) {

	if (keyDown) {
		switch (key) {

		case 0x57:
			//W key
			rotateX = 0;
			break;

		case 0x53:
			//S key
			rotateX = 0;
			break;

		case 0x41:
			//A key
			rotateY = 0;
			break;

		case 0x44:
			//D key
			rotateY = 0;
			break;

		case 0x51:
			//Q key
			rotateZ = 0;
			break;

		case 0x45:
			//E key
			rotateZ = 0;
			break;

		case 0x52:
			//R key
			zoom = 0;
			break;

		case 0x46:
			//R key
			zoom = 0;
			break;

		case VK_SPACE:
			break;

		case VK_LEFT:
			break;

		case VK_PRIOR:
			break;

		case VK_NEXT:
			break;
		}

		if (key == lastKey)
			keyDown = false;
	}
	return TRUE;
}

//Add code here for updating transformation matrices
void OGLWindow::UpdateTransformationMatrices() {
	

}

void OGLWindow::LoadModels() {

	//Load the teapot model
	models.push_back(new TriangleMesh());
	models[0]->LoadMeshFromOBJFile("Models/teapot.obj");

	//Load the cube
	models.push_back(new TriangleMesh());
	models[1]->LoadMeshFromOBJFile("Models/cube.obj");
	
	//Load the lego model
	models.push_back(new TriangleMesh());
	models[2]->LoadMeshFromOBJFile("Models/ShadowTest.obj");

	totalModels = 3;
}