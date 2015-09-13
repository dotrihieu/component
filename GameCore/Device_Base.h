#ifndef DEVICE_BASE_H
#define DEVICE_BASE_H
#include "common.h"
#include "RenderTarget.h"
#include "XMath.h"
#include "Shader.h"
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include "BlendState.h"

class Device_Base : public Singleton<Device_Base>
{
public:
	enum DeviceFeature
	{
		DEVICE_FEATURE_PACKED_DEPTH_STENCIL,
		DEVICE_FEATURE_DEPTH_TEXTURE,
		DEVICE_FEATURE_COUNT
	};
protected:

	RenderTarget* renderTargetMain;
	std::vector<RenderTarget*> renderTargetStack;
	int m_Width, m_Height;
	Matrix  m_MatrixProjection, /*m_MatrixView,*/ m_MatrixProjection2D, m_MatrixProjection2DLogical, m_MatrixProjectionStore;
	std::vector<Matrix> m_MatrixStack;
	VertexShader* vsActivated;
	PixelShader* psActivated;
	Material* materialActivated;
	VBO* vboActivated[VBO::VBO_TARGET_COUNT]; //Interleave VBO. Not support seperated VBO at this time
	AttributePointer attributePointerActivated[ATTRIBUTE_COUNT]; //without VBO
	Program* programActivated;
	int numTextureActivated;
	int textureSlotActivated[UNIFORM_COUNT];
	Camera* cameraActivated;
	BlendState currentBlendState;
	bool deviceFeatureSupport[DEVICE_FEATURE_COUNT];
	
	float blendFactor[4];
	unsigned int blendSampleMask;
	bool isDrawing2D;

public:
	enum DeviceType
	{
		DEVICE_UNKNOW,
		DEVICE_WP8_XAML,
		DEVICE_OPENGL,
	};
	enum PrimitiveType
	{
#ifdef USE_OPENGL
		PRIMITIVE_POINT = GL_POINTS,
		PRIMITIVE_LINE = GL_LINES,
		PRIMITIVE_TRIANGLE_LIST = GL_TRIANGLES,
		PRIMITIVE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		PRIMITIVE_TRIANGLE_FAN = GL_TRIANGLE_FAN,
#else
		PRIMITIVE_POINT,
		PRIMITIVE_LINE,
		PRIMITIVE_TRIANGLE_LIST,
		PRIMITIVE_TRIANGLE_STRIP,
		PRIMITIVE_TRIANGLE_FAN,
#endif
	};
	enum PolygonMode
	{
#ifdef USE_OPENGL
		POLYGON_FILL = GL_FILL,
		POLYGON_LINE = GL_LINE,
#else
		POLYGON_FILL,
		POLYGON_LINE,
#endif
	};
	
	static void Init(DeviceType type, int width, int height, void* params[]);

	virtual Device_Base::DeviceType GetDeviceType()
	{
		return Device_Base::DeviceType::DEVICE_UNKNOW;
	}
	virtual void Resume()
	{
	}
	virtual void Suspend()
	{
	}
	static void Release()
	{
		for(std::vector<RenderTarget*>::iterator it = instance->renderTargetStack.begin(); it != instance->renderTargetStack.end(); ++it)
		{
			SafeDelete(*(it));
		}
		instance->renderTargetStack.clear();
		SafeRelease(instance);
	}
	virtual int CreateVertexBufferFromVerticesData(float* verticesData, unsigned int dataLength, void** vBufferIdOuput)
	{
		return 0;
	}
	virtual void FlipBuffer() = 0;
	virtual int GetWidth()
	{
		return m_Width;
	}
	virtual int GetHeight()
	{
		return m_Height;
	}
	virtual void SetClearColorBuffer(float r, float g, float b, float a) = 0;
	virtual void ClearColorDepthStencilBuffer(bool color, bool depth, bool stencil) = 0;
	virtual void CreateVertexShaderFromCompiledResource(char* fileName)
	{
	}
	virtual void SetActiveVertexShader(VertexShader* vs) = 0;
	virtual void SetActivePixelShader(PixelShader* ps) = 0;
	virtual void SetActiveMaterial(Material* m);
	virtual VertexShader* GetActiveVertexShader()
	{
		return vsActivated;
	}
	virtual PixelShader* GetActivePixelShader()
	{
		return psActivated;
	}
	virtual Material* GetActiveMaterial()
	{
		return materialActivated;
	}
	virtual void PushMatrix();
	virtual void PopMatrix();
	virtual void SetIdentityMatrix();
	virtual void Transform(const Matrix &matrix);
	virtual void TransformLocal(const Matrix &matrix);
	virtual void Translate(float x, float y, float z);
	virtual void Scale(float x, float y, float z);
	virtual void RotateOz(float radian, float x, float y);
	virtual void RotateOz(float radian);
	virtual void RotateOy(float radian);
	virtual void RotateOx(float radian);
	virtual void RotateAxis(float radian, float x, float y, float z);
	virtual void SetProjectionPerspectiveMatrix(float angleFovy, float ratio, float fNear, float fFar);
	virtual void SetProjectionOrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar); //zNear must not equal 0.0f
	virtual void SetProjectionOrthoCenterMatrix(float width, float height, float zNear, float zFar);
	virtual void SetProjectionMatrix(Matrix  &matrix);
	//virtual void SetViewMatrix(float xEye, float yEye, float zEye, float xDes, float yDes, float zDes, float xUp, float yUp, float zUp);
	//virtual void SetViewMatrix(Matrix  &matrix);
	virtual void SetViewport(float x, float y, float width, float height) = 0;
	virtual Matrix  GetMatrixProjection()
	{
		return m_MatrixProjection;
	}
	virtual Matrix  GetMatrixView()
	{
		//return m_MatrixView;
		if (instance->cameraActivated)
		{
			return instance->cameraActivated->GetViewMatrix();
		}
		else
		{
			Matrix matrix;
			matrix.SetIdentity();
			return matrix;
		}
	}
	virtual Matrix  GetMatrixWorld()
	{
		return m_MatrixStack.back();
	}
	virtual Matrix  GetMatrixWorldViewProjection();

	virtual void Draw(Device_Base::PrimitiveType primitiveType, unsigned int startIndex, unsigned int count) = 0;
	virtual void BeginDraw2D();
	virtual void BeginDraw2DLogical();
	virtual void EndDraw2D();
	
	#ifdef USE_DIRECTX
	virtual void Draw2DTextureScreenCoord(Texture* texture, Sampler* sampler, RectScreenF des, RectScreenF source);
	virtual void Draw2DTextureLogicalCoord(Texture* texture, Sampler* sampler, RectScreenF des, RectScreenF source); // height: -1.0 -> 1.0
	virtual void Draw2DTextureFullViewport(Texture* texture, Sampler* sampler);
	virtual void DrawXColladaDX(XColladaDX* mesh);
	#else
	virtual void Draw2DTextureScreenCoord(Texture* texture, RectScreenF des, RectScreenF source) = 0;
	virtual void Draw2DTextureLogicalCoord(Texture* texture, RectScreenF des, RectScreenF source) = 0; // height: -1.0 -> 1.0
	virtual void Draw2DTextureFullViewport(Texture* texture) = 0;
	#endif

	virtual void EnableDepthTest() = 0;
	virtual void DisableDepthTest() = 0;
	virtual void EnableStencilTest() = 0;
	virtual void DisableStencilTest() = 0;
	virtual void EnableBlend() = 0;
	virtual void DisableBlend() = 0;
	virtual void EnableFaceCulling() = 0;
	virtual void DisableFaceCulling() = 0;
	virtual RenderTarget* GetCurrentRenderTarget();
	virtual void PushRenderTarget(RenderTarget* rt) = 0;
	virtual void PopRenderTarget() = 0;
	virtual void CreateVertexShaderFromMemory(char* data, int length, VertexShader* vs) = 0;
	virtual void CreatePixelShaderFromMemory(char* data, int length, PixelShader* ps) = 0;
	virtual void CreateTexture(Texture *texture) = 0;
	virtual void CreateTextureFromMemory(Texture *texture, const char* data) = 0;
	virtual void DeleteTexture(Texture *texture) = 0;
	virtual void CreateProgramFromShader(Program* program) = 0;
	virtual void CreateVBO(VBO* vbo, const char* data, int size) = 0;
	virtual void DeleteVBO(VBO* vbo) = 0;
	virtual void BindVBO(VBO* vbo) = 0;
	virtual void UnbindVBO(VBO::VBO_TARGET target) = 0;
	virtual void UpdateVertexAttribPointer() = 0;
	virtual int BaseToSpecific_VBO_TARGET(VBO::VBO_TARGET target) = 0;
	virtual void UseProgram(Program* prg) = 0;
	virtual void SetUniformTexture(UNIFORM_TYPE type, Texture* texture) = 0;
	virtual void SetUniformMatrix(UNIFORM_TYPE type, Matrix &matrix) = 0;
	virtual void SetUniformVec4f(UNIFORM_TYPE type, Vector4f &vertex) = 0;
	virtual void SetUniformVec2f(UNIFORM_TYPE type, const float &v1, const float &v2) = 0;
	Camera* GetCameraActivated();
	void SetCameraActivated(Camera* camera);
	virtual void DrawArray(PrimitiveType type, int offset, int count) = 0;
	virtual void DrawElement(PrimitiveType type, int count, const void* indices) = 0; //default data type unisgned int
	virtual void SetBlendState(const BlendState &blendState) = 0;
	virtual void CreateRenderTarget(RenderTarget* rt) = 0;
	virtual bool IsFeatureSupported(DeviceFeature feature)
	{
		return deviceFeatureSupport[feature];
	}
	void SetAttributePointer(ATTRIBUTE_TYPE type, void* pdata, int stride)
	{
		attributePointerActivated[type].pdata = pdata;
		attributePointerActivated[type].stride = stride;
	}
	virtual void SetPolygonMode(PolygonMode mode) = 0;

protected:
	virtual void InitSpecificDevice()
	{
	}
	virtual void InitMatrixStack();
};
#endif