#include "Camera.h"

Camera::Camera(void)
{

}

Camera::~Camera(void)
{
}

Matrix Camera::update()
{
	Matrix mRotate,	mResult;

	return mResult;
}

void Camera::Move(float dx, float dy, float dz)
{
	cameraPos.x += dx;
	cameraPos.y += dy;
	cameraPos.z += dz;
	UpdateViewMatrix();
}

void Camera::MoveLeft(float ds)
{
	Vector3f right = Cross(direction, up);
	right *= -ds;
	Move(right.x, right.y, right.z);
}
void Camera::MoveRight(float ds)
{
	Vector3f right = Cross(direction, up);
	right *= ds;
	Move(right.x, right.y, right.z);
}
void Camera::MoveForward(float ds)
{
	Vector3f m = direction;
	m *= ds;
	Move(m.x, m.y, m.z);
}
void Camera::MoveBackward(float ds)
{
	Vector3f m = direction;
	m *= -ds;
	Move(m.x, m.y, m.z);
}

void Camera::MoveForwardFPS(float ds)
{
	Vector3f right = Cross(direction, up);
	Vector3f forward = Cross(up, right);
	forward *= ds;
	Move(forward.x, forward.y, forward.z);
}
void Camera::MoveBackwardFPS(float ds)
{
	Vector3f right = Cross(direction, up);
	Vector3f forward = Cross(up, right);
	forward *= -ds;
	Move(forward.x, forward.y, forward.z);
}

void Camera::rotateOy(float radian)
{
	Matrix rotate;
	rotate.SetRotateOy(radian);
	Vector4f directionTemp;
	directionTemp.x = direction.x;
	directionTemp.y = direction.y;
	directionTemp.z = direction.z;
	directionTemp.w = 1.0;

	directionTemp = rotate * directionTemp;
	direction.x = directionTemp.x;
	direction.y = directionTemp.y;
	direction.z = directionTemp.z;
	UpdateViewMatrix();
}
void Camera::rotateOx(float radian)
{
	Matrix rotate;
	rotate.SetRotateOx(radian);
	Vector4f directionTemp;
	directionTemp.x = direction.x;
	directionTemp.y = direction.y;
	directionTemp.z = direction.z;
	directionTemp.w = 1.0;

	directionTemp = rotate * directionTemp;
	direction.x = directionTemp.x;
	direction.y = directionTemp.y;
	direction.z = directionTemp.z;
	UpdateViewMatrix();
}

void Camera::SetCameraLookAt(float eyeX, float eyeY, float eyeZ, float desX, float desY, float desZ, float upX, float upY, float upZ)
{
	cameraPos.x = eyeX;
	cameraPos.y = eyeY;
	cameraPos.z = eyeZ;
	//------------------
	direction.x = desX - eyeX;
	direction.y = desY - eyeY;
	direction.z = desZ - eyeZ;
	direction = Normalize(direction);

	up.x = upX;
	up.y = upY;
	up.z = upZ;
	up = Normalize(up);
	
	UpdateViewMatrix();
}
void Camera::UpdateViewMatrix()
{
	/*
	Vector3f side;
	Matrix matrix;

	//------------------
	//Side = forward x up
	side = Cross(direction, up);
	side = Normalize(side);
	//------------------
	//Recompute up as: up = side x forward
	Vector3f newUp;
	newUp = Cross(side, direction);

	//------------------
	matrix.data[0][0] = side.x;
	matrix.data[1][0] = side.y;
	matrix.data[2][0] = side.z;
	matrix.data[3][0] = 0.0;
	//------------------
	matrix.data[0][1] = newUp.x;
	matrix.data[1][1] = newUp.y;
	matrix.data[2][1] = newUp.z;
	matrix.data[3][1] = 0.0;
	//------------------
	matrix.data[0][2] = -direction.x;
	matrix.data[1][2] = -direction.y;
	matrix.data[2][2] = -direction.z;
	matrix.data[3][2] = 0.0;
	//------------------
	matrix.data[0][3] = 0.0;
	matrix.data[1][3] = 0.0;
	matrix.data[2][3] = 0.0;
	matrix.data[3][3] = 1.0;
	//------------------
	Matrix translate;
	translate.SetTranslate(-cameraPos.x, -cameraPos.y,-cameraPos.z);
	viewMatrix = translate*matrix;
	*/
	Vector3f ndirection;
	ndirection.x = -direction.x;
	ndirection.y = -direction.y;
	ndirection.z = -direction.z;
    Vector3f zaxis = Normalize(ndirection);
    Vector3f xaxis = Normalize(Cross(up, zaxis));
    Vector3f yaxis = Cross(zaxis, xaxis);
    viewMatrix[0][0] = xaxis.x;
    viewMatrix[0][1] = yaxis.x;
    viewMatrix[0][2] = zaxis.x;
    viewMatrix[0][3] = 0;

    viewMatrix[1][0] = xaxis.y;
    viewMatrix[1][1] = yaxis.y;
    viewMatrix[1][2] = zaxis.y;
    viewMatrix[1][3] = 0;

    viewMatrix[2][0] = xaxis.z;
    viewMatrix[2][1] = yaxis.z;
    viewMatrix[2][2] = zaxis.z;
    viewMatrix[2][3] = 0;

    viewMatrix[3][0] = -Dot(xaxis, cameraPos);
    viewMatrix[3][1] = -Dot(yaxis, cameraPos);
    viewMatrix[3][2] = -Dot(zaxis, cameraPos);
    viewMatrix[3][3] = 1;
	
}
Matrix Camera::GetViewMatrix()
{
	return viewMatrix;
}