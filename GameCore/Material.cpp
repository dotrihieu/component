#include "Material.h"
#include "DXDevice.h"

bool Material::SetUniformVertexByType(UINT type, void* data)
{
	if(vs->UniformByTypeOffset[type] < 0)
	{
		return false;
	}
	{
		memcpy(uniformDataStoreVertex + vs->UniformByTypeOffset[type],data,sizeof(float)*16);
		needUpdateUniformBufferVertex = true;
		return true;
	}
}
bool Material::SetUniformPixelByType(UINT type, void* data)
{
	if(ps->UniformByTypeOffset[type] < 0)
	{
		return false;
	}
	{
		memcpy(uniformDataStorePixel + ps->UniformByTypeOffset[type],data,sizeof(float)*16);
		needUpdateUniformBufferPixel = true;
		return true;
	}
}
bool Material::PushMaterialDataVertex()
{
	if(needUpdateUniformBufferVertex)
	{
		UpdateUniformBufferVertex();
		needUpdateUniformBufferVertex = false;
	}
	if(vs->uniformBuffer) DXDevice::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &(vs->uniformBuffer));
	return true;
}
bool Material::PushMaterialDataPixel()
{
	if(needUpdateUniformBufferPixel)
	{
		UpdateUniformBufferPixel();
		needUpdateUniformBufferPixel = false;
	}
	if(ps->uniformBuffer) DXDevice::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(0, 1, &(ps->uniformBuffer));
	//DXDevice::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, shaderResourcePixel.size(), &shaderResourcePixel[0]);
	return true;
}
bool Material::UpdateUniformBufferVertex()
{
	if(uniformDataStoreVertex == NULL)
	{
		return true;
	}
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;


	// Lock the constant buffer so it can be written to.
	result = DXDevice::GetInstance()->GetDeviceContext()->Map(vs->uniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	void* dataPtr = mappedResource.pData;

	// Copy the matrices into the constant buffer.
	memcpy((BYTE*)dataPtr, uniformDataStoreVertex, vs->sizeOfShaderUniforms);

	// Unlock the constant buffer.
	DXDevice::GetInstance()->GetDeviceContext()->Unmap(vs->uniformBuffer, 0);

	return true;
}
bool Material::UpdateUniformBufferPixel()
{
	if(uniformDataStorePixel == NULL)
	{
		return true;
	}
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;


	// Lock the constant buffer so it can be written to.
	result = DXDevice::GetInstance()->GetDeviceContext()->Map(ps->uniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	void* dataPtr = mappedResource.pData;

	// Copy the matrices into the constant buffer.
	memcpy((BYTE*)dataPtr, uniformDataStorePixel, ps->sizeOfShaderUniforms);

	// Unlock the constant buffer.
	DXDevice::GetInstance()->GetDeviceContext()->Unmap(ps->uniformBuffer, 0);

	return true;
}