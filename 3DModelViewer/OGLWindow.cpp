#include "Resource.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "OGLWindow.h"
#include "Matrix4x4.h"
#include "Vector4D.h"
#include <cmath>

OGLWindow::OGLWindow()
{
	
}

OGLWindow::~OGLWindow()
{
	//Clean up the renderable
	delete m_cube;
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

	//Instantiate a Renderable as OGLCube
	m_cube = new OGLCube();
	m_model = new TriangleMesh();
	//m_model->LoadMeshFromOBJFile("Models/cube.obj");
	m_model->LoadMeshFromOBJFile("Models/teapot.obj");
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
	
	//Camera variables
	Vector4D cameraPosition(0, 0, -10);
	m_model->UpdateCameraPosition(&cameraPosition);
	Vector4D upVector(0, 1, 0);
	Vector4D lookAt(0, 0, 0);
	Vector4D viewVector = lookAt - cameraPosition;
	viewVector.Normalise();

	//TODO:
	//1. Set the matrix 'transformation' as a rotation about the Z axis
	//
	//2. Set the matrix 'transformation' as a more complex transformation by concatenating, e.g. Rz Rx Ry

	//Set the transformation matrix as a 30 degree rotation about the z axis
	
	//Remember, M.T.R.S.

	transformation.SetIdentity();
	Vector4D translationVector(0, 0, sin(time * 0.0174533));

	temporaryTransformation.SetViewMatrix(cameraPosition, viewVector, upVector);
	transformation = transformation * temporaryTransformation;

	temporaryTransformation.SetTranslate(translationVector);
	transformation = transformation * temporaryTransformation;

	temporaryTransformation.SetRotationYAxis(time / 30);
	transformation = transformation * temporaryTransformation;
	temporaryTransformation.SetRotationZAxis(time / 60);
	transformation = transformation * temporaryTransformation;

	temporaryTransformation.SetScale(0.05, 0.05, 0.05);
	transformation = transformation * temporaryTransformation;

	//m_cube->Render(&transformation);
	m_model->Render(&transformation);

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