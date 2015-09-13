#pragma once

#include "XMath.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	Vector3f cameraPos; // current cameraPos
	Vector3f direction;
	Vector3f up;


	Matrix update();
	void SetCameraLookAt(float eyeX, float eyeY, float eyeZ, float desX, float desY, float desZ, float upX, float upY, float upZ);
	void MoveLeft(float ds);
	void MoveRight(float ds);
	void MoveForward(float ds);
	void MoveBackward(float ds);
	void MoveForwardFPS(float ds);
	void MoveBackwardFPS(float ds);
	void Move(float dx, float dy, float dz);
	void rotateOy(float radian);
	void rotateOx(float radian);
	Matrix GetViewMatrix();
	void UpdateViewMatrix();
private:
	Matrix viewMatrix;
};
