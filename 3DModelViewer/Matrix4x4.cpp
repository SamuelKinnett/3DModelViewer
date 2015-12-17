#include <memory.h>
#include <cmath>
#include "Vector4D.h"
#include "Matrix4x4.h"

Matrix4x4::Matrix4x4()
{
	m_deg2rad = 0.0174533;

	SetIdentity();
}

Matrix4x4::Matrix4x4(const Matrix4x4& rhs)
{
	m_deg2rad = 0.0174533;

	size_t matrix_size = sizeof(m_element);

	memcpy_s((&m_element[0][0]), matrix_size, rhs.ToPtr(), matrix_size);

}

Matrix4x4::~Matrix4x4()
{

}

const float* Matrix4x4::ToPtr() const
{
	return &(m_element[0][0]);
}

void Matrix4x4::SetZero()
{
	memset(&m_element, 0, sizeof(m_element));
}

void Matrix4x4::SetIdentity()
{
	SetZero();
	m_element[0][0] = m_element[1][1] = m_element[2][2] = m_element[3][3] = 1.0f;
}

Matrix4x4 Matrix4x4::operator* (const Matrix4x4& rhs) const
{
	Matrix4x4 result;

	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			//Here, we calculate the value of result[x][y]
			//This function will obviously only work with 4x4
			//matrices

			//The x and y integers are used to determine the
			//row for this matrix and the column for the rhs.
			result.m_element[x][y] =
				m_element[0][y] * rhs.m_element[x][0] +
				m_element[1][y] * rhs.m_element[x][1] +
				m_element[2][y] * rhs.m_element[x][2] +
				m_element[3][y] * rhs.m_element[x][3];
		}
	}
	
	return result;
}

Vector4D Matrix4x4::operator* (const Vector4D& rhs) const
{
	Vector4D result;
	float temporaryVector[4];	//Stores the vector's values
								// until such time as we can
								// assign them.
	result = rhs;
	
	for (int currentRow = 0; currentRow < 4; ++currentRow) {
		temporaryVector[currentRow] =
			m_element[0][currentRow] * rhs[0] +
			m_element[1][currentRow] * rhs[1] +
			m_element[2][currentRow] * rhs[2] +
			m_element[3][currentRow] * rhs[3];
	}
	
	result.SetVector(temporaryVector[0], temporaryVector[1],
		temporaryVector[2], temporaryVector[3]);

	return result;
}

void Matrix4x4::SetRotationXAxis(float degrees)
{
	//TODO: Set this matrix as a rotation matrix representing a rotation about the X axis by degrees
	
	//x axis rotation matrix looks like this:
	//	1		0		0		0
	//	0		cos(x)	-sin(x)	0
	//	0		sin(x)	cos(x)	0
	//	0		0		0		1
	
	SetIdentity();
	m_element[1][1] = cos(degrees * m_deg2rad);
	m_element[2][1] = sin(degrees * m_deg2rad) * -1;
	m_element[1][2] = sin(degrees * m_deg2rad);
	m_element[2][2] = cos(degrees * m_deg2rad);
}

void Matrix4x4::SetRotationYAxis(float degrees)
{
	//TODO: Set this matrix as a rotation matrix representing a rotation about the Y axis by degrees

	//y axis rotation matrix looks like this:
	//	cos(x)	0		sin(x)	0
	//	0		1		0		0
	//	-sin(x)	0		cos(x)	0
	//	0		0		0		1

	SetIdentity();
	m_element[0][0] = cos(degrees * m_deg2rad);
	m_element[2][0] = sin(degrees * m_deg2rad);
	m_element[0][2] = sin(degrees * m_deg2rad) * -1;
	m_element[2][2] = cos(degrees * m_deg2rad);
}

void Matrix4x4::SetRotationZAxis(float degrees)
{
	//TODO: Set this matrix as a rotation matrix representing a rotation about the Z axis by degrees

	//z axis rotation matrix looks like this:
	//	cos(x)	-sin(x)	0		0
	//	sin(x)	cos(x)	0		0
	//	0		0		1		0
	//	0		0		0		1

	SetIdentity();
	m_element[0][0] = cos(degrees * m_deg2rad);
	m_element[1][0] = sin(degrees * m_deg2rad) * -1;
	m_element[0][1] = sin(degrees * m_deg2rad);
	m_element[1][1] = cos(degrees * m_deg2rad);
}

void Matrix4x4::SetTranslate(const Vector4D& tvec)
{
	//TODO: Set this matrix as a translation matrix given the translation vector tvec

	SetIdentity();
	m_element[3][0] = tvec[0];
	m_element[3][1] = tvec[1];
	m_element[3][2] = tvec[2];
	m_element[3][3] = tvec[3];
}

void Matrix4x4::SetScale(float sx, float sy, float sz)
{
	//TODO: Set this matrix as a scaling matrix given the scaling factors sx, sy, sz

	SetIdentity();
	m_element[0][0] = sx;
	m_element[1][1] = sx;
	m_element[2][2] = sx;
}

void Matrix4x4::SetViewMatrix(const Vector4D& camera_position, const Vector4D& view_vector, const Vector4D& up_vector)
{
	//TODO: Slightly more advanced
	//Set this matrix as a view matrix based on the given camera_position, view_vector and up_vector

	//View matrix looks like this
	//	Rx		Ry		Rz		EyeX
	//	Ux		Uy		Uz		EyeY
	//	Vx		Vy		Vz		EyeZ
	//	0		0		0		1

	Vector4D right_vector = view_vector.CrossProduct(up_vector);
	right_vector.Normalise();

	SetIdentity();

	m_element[0][0] = right_vector[0]; 
	m_element[1][0] = right_vector[1];
	m_element[2][0] = right_vector[2];
	m_element[3][0] = camera_position[0];

	m_element[0][1] = up_vector[0];
	m_element[1][1] = up_vector[1];
	m_element[2][1] = up_vector[2];
	m_element[3][1] = camera_position[1];

	m_element[0][2] = view_vector[0];
	m_element[1][2] = view_vector[1];
	m_element[2][2] = view_vector[2];
	m_element[3][2] = camera_position[2];

}

//Fuck that awful method, use this one instead!
void Matrix4x4::SetLookAt(Vector4D eyePosition, Vector4D target, Vector4D up) {
	
	SetIdentity();

	Vector4D forwardVector = eyePosition - target;
	forwardVector.Normalise();
	Vector4D rightVector = forwardVector.CrossProduct(up);
	rightVector.Normalise();
	Vector4D upVector = rightVector.CrossProduct(forwardVector);
	upVector.Normalise();

	//Set the matrix
	m_element[0][0] = rightVector[0];
	m_element[1][0] = rightVector[1];
	m_element[2][0] = rightVector[2];

	m_element[0][1] = upVector[0];
	m_element[1][1] = upVector[1];
	m_element[2][1] = upVector[2];

	m_element[0][2] = forwardVector[0];
	m_element[1][2] = forwardVector[1];
	m_element[2][2] = forwardVector[2];

	//Set the position of the camera, accounting for the fact that it must be inverted

	m_element[3][0] = rightVector.DotProduct(eyePosition) * -1;
	m_element[3][1] = upVector.DotProduct(eyePosition) * -1;
	m_element[3][2] = forwardVector.DotProduct(eyePosition) * -1;


}