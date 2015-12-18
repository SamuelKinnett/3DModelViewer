#pragma once

#include <Windows.h>
#include <vector>
#include "OGLRectangle.h"
#include "OGLCube.h"
#include "TriangleMesh.h"
#include "OBJFileReader.h"
#include "Model.h"

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

		std::vector<Model*> sceneModels;	//All models to be rendered.

		//TESTING
		float time;

		void UpdateTransformationMatrices();	//The user can add code here to update their stored
												// transformation matrices. This can be used to allow
												// animations.
		void LoadModels();	//Loads every .obj file into the models list.

		//The following variables are used with keyboard input for the manipulation of models.
		
		bool keyDown;
		WPARAM lastKey;

		int totalModels;
		int focusedModel;

		float rotateX;
		float rotateY;
		float rotateZ;
		float zoom;


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
		BOOL		KeyDown(WPARAM key);
		BOOL		KeyUp(WPARAM key);
};