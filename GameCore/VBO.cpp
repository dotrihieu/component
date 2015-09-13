#include "VBO.h"
#include "Device_Base.h"
VBO::~VBO()
{
	Device_Base::GetInstance()->DeleteVBO(this);
}