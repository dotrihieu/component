#include "XMath.h"
Vector3f Normalize(const Vector3f &v)
{
	Vector3f result;
	float magnitude = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	result.x = v.x/magnitude;
	result.y = v.y/magnitude;
	result.z = v.z/magnitude;
	return result;
}
Vector3f Cross(const Vector3f &u, const Vector3f &v)
{
	Vector3f result;
	result.x = u.y*v.z - u.z*v.y;
	result.y = u.z*v.x - u.x*v.z;
	result.z = u.x*v.y - u.y*v.x;
	return result;
}
float Dot(const Vector3f &u, const Vector3f &v)
{
	return u.x*v.x + u.y*v.y+u.z*v.z;
}
void Matrix::SetRotateAxis(float radian, float x, float y, float z)																		
{
	Vector3f n = Normalize(Vector3f(x, y, z));
	SetRotateNormalizedAxis(radian, n.x, n.y, n.z);
}
void Matrix::SetView(float xEye, float yEye, float zEye, float xDes
		, float yDes, float zDes, float xUp, float yUp, float zUp)
{
	Vector3f cameraPosition(xEye,yEye,zEye);
	Vector3f cameraTarget(xDes,yDes,zDes);
	Vector3f cameraUpVector(xUp,yUp,zUp);
	Vector3f zaxis = Normalize(cameraTarget - cameraPosition);
	Vector3f xaxis = Normalize(Cross(cameraUpVector, zaxis));
	Vector3f yaxis = Cross(zaxis, xaxis);
	data[0][0] = xaxis.x;
	data[0][1] = yaxis.x;
	data[0][2] = zaxis.x;
	data[0][3] = 0;
	data[1][0] = xaxis.y;
	data[1][1] = yaxis.y;
	data[1][2] = zaxis.y;
	data[1][3] = 0;
	data[2][0] = xaxis.z;
	data[2][1] = yaxis.z;
	data[2][2] = zaxis.z;
	data[2][3] = 0;
	data[3][0] = -Dot(xaxis, cameraPosition);
	data[3][1] = -Dot(yaxis, cameraPosition);
	data[3][2] = -Dot(zaxis, cameraPosition);
	data[3][3] = 1;
	/*
		xaxis.x           yaxis.x           zaxis.x          0
		xaxis.y           yaxis.y           zaxis.y          0
		xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, cameraPosition)  -dot(yaxis, cameraPosition)  -dot(zaxis, cameraPosition)  1
	*/

}