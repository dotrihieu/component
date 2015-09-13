#ifndef MYMATH_H
#define MYMATH_H
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef USE_DIRECTX
#include "directxmath.h"
#endif
struct Vector2f
{
public:
	float x, y;
	Vector2f()
	{
		this->x = 0.0f;
		this->y = 0.0f;
	}
	Vector2f(const float &x, const float &y)
	{
		this->x = x;
		this->y = y;
	}
	const Vector2f operator- (const Vector2f &other) const
	{
		return Vector2f(this->x - other.x, this->y - other.y);
	}
	const Vector2f operator+ (const Vector2f &other) const
	{
		return Vector2f(this->x + other.x, this->y + other.y);
	}
	const Vector2f operator* (const float &value) const
	{
		return Vector2f(this->x * value, this->y * value);
	}
	static const Vector2f GetNomalize(float x, float y)
	{
		Vector2f result(0.0,0.0);
		float length = sqrt(x*x + y*y);
		if(length != 0)
		{
			result.x = x/length;
			result.y = y/length;
		}
		return result;
	}
};
struct Vector4f
{
public:
	float x, y, z, w;
	Vector4f()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
	Vector4f(const float &x, const float &y, const float &z, const float &w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};
struct Matrix
{
private:
	float data[4][4];
	//float *data = new float[16];
	//data[0][0] data[0][1] data[0][2] data[0][3] //DirectX row major, left hand, OpenGL column major, right hand, DX & GL have same matrix layout in memory
	//data[1][0] data[1][1] data[1][2] data[1][3]
	//data[2][0] data[2][1] data[2][2] data[2][3]
	//data[3][0] data[3][1] data[3][2] data[3][3]
public:
	const Matrix& operator=( const Matrix& rhs )
	{
		memcpy(this->data,rhs.data, 16*sizeof(float));
		return *this;
	}
	float* const& operator[](const int index) const
	{
          return (float*)data[index];
    }
	operator float*()
	{
		return data[0];
	}
	void SetIdentity()
	{
		memset(data,0,sizeof(data));
		data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0f;
	}
	void SetTranslate(float x, float y, float z)
	{
		 SetIdentity();
		 data[3][0] = x;
		 data[3][1] = y;
		 data[3][2] = z;
	}
	void SetScale(float x, float y, float z)
	{
		memset(data,0,sizeof(data));
		data[0][0] = x;
		data[1][1] = y;
		data[2][2] = z;
		data[3][3] = 1.0f;
	}
	void SetRotateOx(float radian)
	{
		memset(data,0,sizeof(data));
		data[0][0] = 1.0f;
		data[1][1] = cos(radian);
		data[2][2] = data[1][1];
		data[1][2] = sin(radian);
		data[2][1] = -data[1][2];
		data[3][3] = 1.0f;
	}
	void SetRotateOy(float radian)
	{
		memset(data,0,sizeof(data));
		data[0][0] = cos(radian);
		data[1][1] = 1.0f;
		data[2][0] = sin(radian);
		data[0][2] = -data[2][0];
		data[2][2] = data[0][0];
		data[3][3] = 1.0f;
	}
	void SetRotateOz(float radian)
	{
		memset(data,0,sizeof(data));
		data[0][0] = cos(radian);
		data[1][1] = data[0][0];
		data[0][1] = sin(radian);
		data[1][0] = -data[0][1];
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
	}
	void SetRotateNormalizedAxis(float radian, float x, float y, float z) //https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml 
																			//http://stackoverflow.com/questions/1263072/changing-a-matrix-from-right-handed-to-left-handed-coordinate-system
	{
		memset(data,0,sizeof(data));

		float c = cos(radian);
		float OneSubCosA = 1.0f - c;
		float s = sin(radian);
		
		data[0][0] = x*x*OneSubCosA + c; data[0][1] = x*y*OneSubCosA - z*s; data[0][2] = -(x*z*OneSubCosA + y*s);
		data[1][0] = y*x*OneSubCosA + z*s; data[1][1] = y*y*OneSubCosA + c; data[1][2] = -(y*z*OneSubCosA - x*s);
		data[2][0] = -(x*z*OneSubCosA - y*s); data[2][1] = -(y*z*OneSubCosA + x*s); data[2][2] = z*z*OneSubCosA + c;
		data[3][3] = 1.0f;
	}
	void SetRotateAxis(float radian, float x, float y, float z);
	void SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		/* //OpenGL
		memset(data,0,sizeof(data));
		data[0][0] = 2.0/(right - left);
		data[1][1] = 2.0/(top - bottom);
		data[2][2] = -2.0/(zFar - zNear);
		data[3][3] = 1.0;
		data[3][0] = -(right + left)/(right - left);
		data[3][1] = -(top + bottom)/(top - bottom);
		data[3][2] = -(zFar + zNear)/(zFar - zNear);
		*/
		memset(data,0,sizeof(data));
		data[0][0] = 2.0f/(right - left);
		data[1][1] = 2.0f/(top - bottom);
		data[2][2] = -2.0f/(zFar - zNear);
		data[3][0] = (left +right )/(left - right);
		data[3][1] = (top + bottom)/(bottom - top);
		data[3][2] = (zNear + zFar) / (zNear - zFar);
		data[3][3] = 1.0f;
	}
	void SetOrthoCentered(float width, float height, float zNear, float zFar)
	{
		/* //OpenGL
		memset(data,0,sizeof(data));
		data[0][0] = 2.0/(right - left);
		data[1][1] = 2.0/(top - bottom);
		data[2][2] = -2.0/(zFar - zNear);
		data[3][3] = 1.0;
		data[3][0] = -(right + left)/(right - left);
		data[3][1] = -(top + bottom)/(top - bottom);
		data[3][2] = -(zFar + zNear)/(zFar - zNear);
		*/
		memset(data,0,sizeof(data));
		data[0][0] = 2.0f/width;
		data[1][1] = 2.0f/height;
		data[2][2] = -2.0f/(zFar - zNear);
		data[3][2] = (zNear + zFar) / (zNear - zFar);
		data[3][3] = 1.0f;
	}
	void SetPerspective(float width, float height, float zNear, float zFar)
	{
		memset(data,0,sizeof(data));
		data[0][0] = 2.0f*zNear/width;
		data[1][1] = 2.0f*zNear/height;
		data[2][2] = (zNear + zFar) / (zNear - zFar);
		data[3][2] =  - 2.0f*zNear*zFar/(zFar - zNear);
		data[2][3] = -1.0f;
	}
	/*
	void SetPerspectiveFovy(const float near_plane, // Distance to near clipping 
                                         // plane
                 const float far_plane,  // Distance to far clipping 
                                         // plane
                 const float fov_horiz,  // Horizontal field of view 
                                         // angle, in radians
                 const float fov_vert)   // Vertical field of view 
                                         // angle, in radians
	{
		float    h, w, Q;

		w = 1.0f/tan(fov_horiz*0.5f);  // 1/tan(x) == cot(x)
		h = 1.0f/tan(fov_vert*0.5f);   // 1/tan(x) == cot(x)
		Q = far_plane/(far_plane - near_plane);

		memset(data,0,sizeof(data));

		data[0][0] = w;
		data[1][1] = h;
		data[2][2] = Q;
		data[3][2] = -Q*near_plane;
		data[2][3] = 1;
	}
	*/
	void SetPerspectiveFovy( const float fovy, float ratio, const float near_plane, const float far_plane)
	{
		float    f, Q;

		f = 1.0f/tan(fovy*0.5f);  // 1/tan(x) == cot(x)

		memset(data,0,sizeof(data));

		data[0][0] = f/ratio;
		data[1][1] = f;
		data[2][2] = (near_plane + far_plane) / (near_plane - far_plane);
		data[3][2] = 2.0f * near_plane * far_plane / (near_plane - far_plane);
		data[2][3] = -1.0f;
	}
	void SetView(float xEye, float yEye, float zEye, float xDes
		, float yDes, float zDes, float xUp, float yUp, float zUp);
	const Matrix operator* (const Matrix &other)  const
	{
		Matrix result;
		#ifdef USE_DIRECTX
		DirectX::XMMATRIX a(*(this->data));
		DirectX::XMMATRIX b(*(other.data));
		DirectX::XMMATRIX c = a*b;
		memcpy(result.data,&c,sizeof(result.data));
		#else
		for(int i = 0;i<4;i++)
		{
			for(int j = 0;j<4;j++)
			{
				result.data[i][j] = this->data[0][j]*other.data[i][0] + this->data[1][j]*other.data[i][1] + this->data[2][j]*other.data[i][2] + this->data[3][j]*other.data[i][3];
			}
		}
		#endif
		return result;
	}
	const Vector4f operator* (const Vector4f &other)  const
	{
		Vector4f r;
		r.x = this->data[0][0]*other.x + this->data[1][0]*other.y + this->data[2][0]*other.z + this->data[3][0]*other.w;
		r.y = this->data[0][1]*other.x + this->data[1][1]*other.y + this->data[2][1]*other.z + this->data[3][1]*other.w;
		r.z = this->data[0][2]*other.x + this->data[1][2]*other.y + this->data[2][2]*other.z + this->data[3][2]*other.w;
		r.w = this->data[0][3]*other.x + this->data[1][3]*other.y + this->data[2][3]*other.z + this->data[3][3]*other.w;

		return r;
	}
	static void MatrixMultiply(const Matrix * a, const Matrix * b, Matrix * result)
	{
		#ifdef USE_DIRECTX
		DirectX::XMMATRIX a(*(this->data));
		DirectX::XMMATRIX b(*(other.data));
		DirectX::XMMATRIX c = a*b;
		memcpy(result.data,&c,sizeof(result->data));
		#else
		for(int i = 0;i<4;i++)
		{
			for(int j = 0;j<4;j++)
			{
				result->data[i][j] = a->data[0][j]*b->data[i][0] + a->data[1][j]*b->data[i][1] + a->data[2][j]*b->data[i][2] + a->data[3][j]*b->data[i][3];
			}
		}
		#endif
	}
	void ConvertRHToLH()
	{
		data[0][2] *= -1.0f;
		data[1][2] *= -1.0f;
		data[2][0] *= -1.0f;
		data[2][1] *= -1.0f;
		//data[2][3] *= -1.0f;// ???http://www.gamedev.net/topic/385001-converting-a-transformation-matrix-from-right-handed-to-left-handed-coord-system/
								//http://msdn.microsoft.com/en-us/library/cc526973.aspx
		data[3][2] *= -1.0f;
	}
	void ConvertZ_upToY_up()
	{
		Matrix matrix;
		memset(matrix.data,0,sizeof(matrix.data));
		matrix.data[0][0] = 1.0f;
		matrix.data[1][2] = 1.0f;
		matrix.data[2][1] = -1.0f;
		matrix.data[3][3] = 1.0f;
		(*this) = (*this) * matrix;
	}
};
struct Rect3DI //3d space
{
	int left, right, top, bottom;
	Rect3DI()
	{
	}
	void FromWidthHeight(int x, int y,  int w, int h)
	{
		left = x;
		right = x + w;
		top = y;
		bottom = y - h;
	}
};
struct Rect3DF //3d space
{
	float left, right, top, bottom;
	Rect3DF()
	{
	}
	Rect3DF(const Rect3DI rect3DI)
	{
		left = (float)rect3DI.left;
		right = (float)rect3DI.right;
		top = (float)rect3DI.top;
		bottom = (float)rect3DI.bottom;
	}
	void FromWidthHeight(float x, float y,  float w, float h)
	{
		left = x;
		right = x + w;
		top = y;
		bottom = y - h;
	}
};
struct RectScreenI //texture coord
{
	int left, right, top, bottom;
	RectScreenI()
	{
	}
	void FromWidthHeight(int x, int y,  int w, int h)
	{
#ifdef USE_DIRECTX
		left = x;
		right = x + w;
		top = y;
		bottom = y + h;
#endif
#ifdef USE_OPENGL
		left = x;
		right = x + w;
		top = y + h;
		bottom = y;
#endif
	}
};
struct RectScreenF //texture coord
{
	float left, right, top, bottom;
	RectScreenF()
	{
	}
	RectScreenF(const RectScreenI rectScreenI)
	{
		left = (float)rectScreenI.left;
		right = (float)rectScreenI.right;
		top = (float)rectScreenI.top;
		bottom = (float)rectScreenI.bottom;
	}
	void FromWidthHeight(float x, float y,  float w, float h)
	{
#ifdef USE_DIRECTX
		left = x;
		right = x + w;
		top = y;
		bottom = y + h;
#endif
#ifdef USE_OPENGL
		left = x;
		right = x + w;
		top = y + h;
		bottom = y;
#endif
	}
};

struct Vector3f
{
public:
	float x, y, z;
	Vector3f()
	{
	}
	Vector3f(const float &x, const float &y, const float &z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	const Vector3f operator- (const Vector3f &other) const
	{
		return Vector3f(this->x - other.x, this->y - other.y, this->z - other.z);
	}
	const Vector3f operator+ (const Vector3f &other) const
	{
		return Vector3f(this->x + other.x, this->y + other.y, this->z + other.z);
	}
	const Vector3f operator* (const Matrix &m) const
	{
		return Vector3f(x*m[0][0] + y*m[1][0] + z*m[2][0] + 1.0f*m[3][0],
						x*m[0][1] + y*m[1][1] + z*m[2][1] + 1.0f*m[3][1],
						x*m[0][2] + y*m[1][2] + z*m[2][2] + 1.0f*m[3][2]);
	}
	const Vector3f operator* (const float &other)  const
	{
		Vector3f r;
		r.x = this->x * other;
		r.y = this->y * other;
		r.z = this->z * other;
		return r;
	}
	const Vector3f operator*= (const float &other)
	{
		this->x = this->x * other;
		this->y = this->y * other;
		this->z = this->z * other;
		return *this;
	}
};
Vector3f Normalize(const Vector3f &v);
Vector3f Cross(const Vector3f &u, const Vector3f &v);
float Dot(const Vector3f &u, const Vector3f &v);
#endif