#include "Model.h"

Model::Model(TriangleMesh* model, Camera* camera, int modelID) {
	this->model = model;
	this->camera = camera;
	this->modelID = modelID;
}

void Model::SetPosition(float x, float y, float z) {
	this->position.SetVector(x, y, z, 1.0f);
}

Vector4D Model::GetPosition() {
	return position;
}

void Model::SetRotation(float rotationX, float rotationY, float rotationZ) {
	this->rotation[0] = rotationX;
	this->rotation[1] = rotationY;
	this->rotation[2] = rotationZ;
}

float* Model::GetRotation() {
	return rotation;
}

void Model::SetScale(float sx, float sy, float sz) {
	this->scale[0] = sx;
	this->scale[1] = sy;
	this->scale[2] = sz;
}

float* Model::GetScale() {
	return scale;
}

void Model::SetModel(TriangleMesh* model) {
	this->model = model;
}

void Model::Render(const Matrix4x4* trans) {

	//Translation 0, an animation.
	Vector4D translationVector();
	Matrix4x4 transformation;
	Matrix4x4 temporaryTransformation;

	transformation.SetIdentity();
	temporaryTransformation.SetIdentity();

	//Apply the view matrix first of all
	temporaryTransformation.SetViewMatrix(this->camera->GetCameraPosition(),
		this->camera->GetViewVector(),
		this->camera->GetUpVector());
	transformation = transformation * temporaryTransformation;

	//Apply transformation matrix
	temporaryTransformation.SetTranslate(position);
	transformation = transformation * temporaryTransformation;

	//Apply rotations
	temporaryTransformation.SetRotationXAxis(this->rotation[0]);
	transformation = transformation * temporaryTransformation;
	temporaryTransformation.SetRotationYAxis(this->rotation[1]);
	transformation = transformation * temporaryTransformation;
	temporaryTransformation.SetRotationZAxis(this->rotation[2]);
	transformation = transformation * temporaryTransformation;

	//Apply scale
	temporaryTransformation.SetScale(this->scale[0],
										this->scale[1],
										this->scale[2]);
	transformation = transformation * temporaryTransformation;

	//Render the model
	this->model->Render(&transformation);
}