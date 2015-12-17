#pragma once

#include <Windows.h>
#include <vector>
#include "OGLRectangle.h"
#include "OGLCube.h"
#include "TriangleMesh.h"
#include "OBJFileReader.h"

//Simple camera structure
struct Camera
{
	Vector4D cameraPosition;
	Vector4D upVector;
	Vector4D lookAt;
	Vector4D viewVector;
};

//Stores a model and a transformation to apply to it.
//Allows for more efficient memory usage by rendering, for example,
// the same teapot model at three different locations.
struct ModelInstance
{
	ModelInstance(TriangleMesh* model, Matrix4x4* transformation) {
		this->model = model;
		this->transformation = transformation;
	}
	TriangleMesh*	model;
	Matrix4x4*		transformation;
};

class OGLWindow
{
	private:
		HWND		m_hwnd;				//handle to a window
		HDC			m_hdc;				//handle to a device context
		HGLRC		m_hglrc;			//handle to a gl rendering context

		int			m_width;			//width of the OGL drawing surface
		int			m_height;			//height of the OGL drawing surface
		
		//This is not an ideal place to hold geometry data
		OGLCube			*m_cube;
		Camera			m_camera;

		std::vector<TriangleMesh*>	models;	//Stores a list of models
		std::vector<Matrix4x4*>		transformations;	//Stores a list of transformations.

		std::vector<ModelInstance*> sceneModels;	//All models to be rendered.

		//TESTING
		float time;

		void UpdateTransformationMatrices();	//The user can add code here to update their stored
												// transformation matrices. This can be used to allow
												// animations.
protected:

		HGLRC CreateOGLContext (HDC hdc);
		BOOL DestroyOGLContext();
		void InitOGLState();

	public:
					OGLWindow();
					OGLWindow(HINSTANCE hInstance, int width, int height);
					~OGLWindow();

		BOOL		InitWindow(HINSTANCE hInstance, int width, int height);
		void		Render();
		void		Resize( int width, int height );
		void		SetVisible( BOOL visible );
		void		DestroyOGLWindow();

		BOOL		MouseLBDown ( int x, int y );
		BOOL		MouseLBUp ( int x, int y );
		BOOL		MouseMove ( int x, int y );
};