#include "Camera.h"

Camera::Camera() {
	this->cameraPosition = Vector4D(0, 0, 0);
	this->upVector = Vector4D(0, 1, 0);
	this->lookAt = Vector4D(0, 0, 0);
}

Camera::Camera(Vector4D position, Vector4D upVector, Vector4D lookAt) {
	this->cameraPosition = position;
	this->upVector = upVector;
	this->lookAt = lookAt;
}

void Camera::SetCameraPosition(float x, float y, float z) {
	this->cameraPosition.SetVector(x, y, z, 1.0f);
}

Vector4D Camera::GetCameraPosition() {
	return cameraPosition;
}

void Camera::SetUpVector(float x, float y, float z) {
	this->upVector.SetVector(x, y, z, 1.0f);
}

Vector4D Camera::GetUpVector() {
	return this->upVector;
}

void Camera::SetLookAt(float x, float y, float z) {
	this->lookAt.SetVector(x, y, z, 1.0f);
}

Vector4D Camera::GetLookAt() {
	return this->lookAt;
}
Vector4D Camera::GetViewVector() {
	Vector4D viewVector;
	viewVector = this->lookAt - this->cameraPosition;
	viewVector.Normalise();

	return viewVector;
}