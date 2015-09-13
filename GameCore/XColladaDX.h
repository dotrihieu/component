#ifndef XCOLLADADX_H
#define XCOLLADADX_H
#include "../../../Tools/XColladaConverter/XColladaConverter/XCollada.h"
#include "Material.h"
#include "Texture.h"
using namespace std;

struct XGeometry
{
	string id;
	string name;
	//string effectName;
	Material* material;
	//vector<ShaderUniform*> uniform;
	ID3D11Buffer* vertexPositionBuffer;
	ID3D11Buffer* vertexTexcoordBuffer;
	unsigned int vertexPositionStride, vertexPositionCount, vertexTexcoordStride, vertexTexcoordCount;
	//ID3D11Buffer* indexBuffer;
	//unsigned int indexStride, indexCount;
	XGeometry()
	{
		material = NULL;
		vertexPositionBuffer = NULL;
		vertexTexcoordBuffer = NULL;
		//indexBuffer = NULL;
	}
};
struct XNode
{
	string id;
	string name;
	Matrix transform;
	vector<XGeometry*> geometries;
};

class XColladaDX
{
public:
	string version;
	vector<XNode*> nodes;
	void InitFromXCollada(char* buffer);
};
string ReadStringFromBinary(char** buffer);
unsigned int ReadUnignedIntFromBinary(char** buffer);
#endif