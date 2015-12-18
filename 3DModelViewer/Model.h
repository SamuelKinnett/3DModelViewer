#ifndef MODEL_H
#define MODEL_H

#include "TriangleMesh.h"
#include "Vector4D.h"
#include "Matrix4x4.h"
#include "Camera.h"

class Model : public Renderable {

private:
	TriangleMesh* model;
	Vector4D position;
	Camera* camera;
	float rotation[3];
	float scale[3];

public:

	int modelID;

	Model(TriangleMesh*, Camera*, int);
	
	void SetPosition(float, float, float);
	Vector4D GetPosition();
	void SetRotation(float, float, float);
	float* GetRotation();
	void SetScale(float, float, float);
	float* GetScale();
	void SetModel(TriangleMesh*);

	void Render(const Matrix4x4* trans = 0);
};

#endif