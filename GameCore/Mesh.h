#ifndef XMESH_H
#define XMESH_H
#include "VBO.h"
#include "Material.h"
#include <vector>
#include <assimp/scene.h>
class Mesh
{
public:
	VBO* vbo;
	VBO* indexVBO;
	Material* material;
	Mesh()
	{
		vbo = NULL;
		indexVBO = NULL;
		material = NULL;
	}
	~Mesh()
	{
		SafeDelete(vbo);
		SafeDelete(indexVBO);
		SafeDelete(material);
	}
};
struct Node
{
	Matrix transformMatrix;
	std::vector<Node*> childNodes;
	std::vector<Mesh*> meshes;
	void InitFromAssimpNode(aiNode* ainode, std::vector<Mesh*> &meshList)
	{
		//memcpy(&(this->transformMatrix), &(ainode->mTransformation), 16 * sizeof(float));
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				this->transformMatrix[i][j] = ainode->mTransformation[j][i];
			}
		}
		for (int i = 0; i < ainode->mNumMeshes; i++)
		{
			meshes.push_back(meshList[ainode->mMeshes[i]]);
		}
		for (int i = 0; i < ainode->mNumChildren; i++)
		{
			Node* child = new Node();
			child->InitFromAssimpNode(ainode->mChildren[i], meshList);
			childNodes.push_back(child);
		}
	}
};
#endif