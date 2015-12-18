#ifndef CAMERA_H
#define CAMERA_H

#include "Vector4D.h"

class Camera {

private:
	Vector4D cameraPosition;
	Vector4D upVector;
	Vector4D lookAt;

public:
	Camera();
	Camera(Vector4D position, Vector4D upVector, Vector4D lookAt);
	void SetCameraPosition(float, float, float);
	Vector4D GetCameraPosition();
	void SetUpVector(float, float, float);
	Vector4D GetUpVector();
	void SetLookAt(float, float, float);
	Vector4D GetLookAt();
	Vector4D GetViewVector();
};

#endif