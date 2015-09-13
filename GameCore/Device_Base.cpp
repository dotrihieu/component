#include "Screen.h"
#ifdef USE_OPENGL
#include "Device_OpenGL.h"
#endif

void Device_Base::Init(DeviceType type, int width, int height, void* params[])
{
	if(type == DEVICE_OPENGL)
	{
		instance = (Device_Base*)new Device_OpenGL();
	}
	GetInstance()->m_Width = width;
	GetInstance()->m_Height = height;
	GetInstance()->SetViewport(0, 0, width, height);
	GetInstance()->programActivated = NULL;
	GetInstance()->numTextureActivated = 0;
	for (int i = 0; i < UNIFORM_COUNT; i++)
	{
		GetInstance()->textureSlotActivated[i] = -1;
	}
	GetInstance()->InitMatrixStack();
	GetInstance()->m_MatrixProjection2D.SetOrthoCentered(width, height, 0.01f, 200.0f);
	GetInstance()->m_MatrixProjection2DLogical.SetOrthoCentered(Screen::GetInstance()->GetRatio()*2.0f, 2.0f, 0.01f, 200.0f);
}
void Device_Base::SetProjectionPerspectiveMatrix(float angleFovy, float ratio, float fNear, float fFar)
{
	m_MatrixProjection.SetPerspectiveFovy(angleFovy, ratio, fNear, fFar);
}
void Device_Base::SetProjectionOrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar) //zNear must not equal 0.0f
{
	m_MatrixProjection.SetOrtho(left,right,bottom,top,zNear,zFar);
}
void Device_Base::SetProjectionOrthoCenterMatrix(float width, float height, float zNear, float zFar) //zNear must not equal 0.0f
{
	m_MatrixProjection.SetOrthoCentered(width, height, zNear, zFar);
}
void Device_Base::SetProjectionMatrix(Matrix  &matrix)
{
	m_MatrixProjection = matrix;
}
/*
void Device_Base::SetViewMatrix(float xEye, float yEye, float zEye, float xDes
		, float yDes, float zDes, float xUp, float yUp, float zUp)
{
	m_MatrixView.SetView(xEye, yEye, zEye, xDes
		, yDes, zDes, xUp, yUp, zUp);
}
void Device_Base::SetViewMatrix(Matrix  &matrix)
{
	m_MatrixView = matrix;
}
*/
void Device_Base::InitMatrixStack()
{
	Matrix identityM;
	identityM.SetIdentity();
	m_MatrixStack.push_back(identityM);
}
void Device_Base::PushMatrix()
{
	Matrix matrix;
	if(m_MatrixStack.size() > 0)
	{
		matrix = m_MatrixStack.back();
	}
	else
	{
		matrix.SetIdentity();
	}
	m_MatrixStack.push_back(matrix);
}
void Device_Base::PopMatrix()
{
	if(m_MatrixStack.size() > 1)
	{
		m_MatrixStack.pop_back();
	}
}
void Device_Base::SetIdentityMatrix()
{
	m_MatrixStack.back().SetIdentity();
}
void Device_Base::Transform(const Matrix &matrix)
{
	m_MatrixStack.back() = m_MatrixStack.back()*matrix;
}
void Device_Base::TransformLocal(const Matrix &matrix)
{
	m_MatrixStack.back() = matrix*m_MatrixStack.back();
}
void Device_Base::Translate(float x, float y, float z)
{
	Matrix trans;
	trans.SetTranslate(x, y, z);
	m_MatrixStack.back() = m_MatrixStack.back()*trans;
}
void Device_Base::Scale(float x, float y, float z)
{
	Matrix scale;
	scale.SetScale(x, y, z);
	m_MatrixStack.back() = m_MatrixStack.back()*scale;
}
void Device_Base::RotateOy(float radian)
{
	Matrix rotate;
	rotate.SetRotateOy(radian);
	m_MatrixStack.back() = m_MatrixStack.back()*rotate;
}
void Device_Base::RotateOz(float radian)
{
	Matrix rotate;
	rotate.SetRotateOz(radian);
	m_MatrixStack.back() = m_MatrixStack.back()*rotate;
}
void Device_Base::RotateOx(float radian)
{
	Matrix rotate;
	rotate.SetRotateOx(radian);
	m_MatrixStack.back() = m_MatrixStack.back()*rotate;
}
void Device_Base::RotateOz(float radian, float x, float y)
{
	Matrix trans;
	trans.SetTranslate(x, y, 0.0);
	Matrix itrans;
	itrans.SetTranslate(-x, -y, 0.0);
	Matrix rotate;
	rotate.SetRotateOz(radian);
	m_MatrixStack.back() = m_MatrixStack.back()*itrans*rotate*trans;
}
void Device_Base::RotateAxis(float radian, float x, float y, float z)
{
	Matrix rotate;
	rotate.SetRotateAxis(radian, x, y, z);
	m_MatrixStack.back() = m_MatrixStack.back()*rotate;
}
Matrix  Device_Base::GetMatrixWorldViewProjection()
{
	Matrix  wvp = m_MatrixStack.back()*GetMatrixView()*m_MatrixProjection;
	return wvp;
}

void Device_Base::SetActiveMaterial(Material* m)
{
	materialActivated = m;
	//SetActiveVertexShader(m->vs);
	//SetActivePixelShader(m->ps);
}

void Device_Base::BeginDraw2D()
{
	if(isDrawing2D == false)
	{
		m_MatrixProjectionStore = m_MatrixProjection;
		m_MatrixProjection = m_MatrixProjection2D;
		DisableDepthTest();
		isDrawing2D = true;
		PushMatrix();
		SetIdentityMatrix();
	}
}
void Device_Base::BeginDraw2DLogical()
{
	if (isDrawing2D == false)
	{
		m_MatrixProjectionStore = m_MatrixProjection;
		m_MatrixProjection = m_MatrixProjection2DLogical;
		DisableDepthTest();
		isDrawing2D = true;
		PushMatrix();
	}
}
void Device_Base::EndDraw2D()
{
	if(isDrawing2D)
	{
		m_MatrixProjection = m_MatrixProjectionStore;
		EnableDepthTest();
		isDrawing2D = false;
		PopMatrix();
	}
}

RenderTarget* Device_Base::GetCurrentRenderTarget()
{
	if (renderTargetStack.size() > 0)
	{
		return renderTargetStack.back();
	}
	else
	{
		return NULL;
	}
}
Camera* Device_Base::GetCameraActivated()
{
	return cameraActivated;
}
void Device_Base::SetCameraActivated(Camera* camera)
{
	cameraActivated = camera;
}