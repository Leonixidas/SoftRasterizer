#pragma once
#include "Singleton.h"
#include <string>


namespace Elite
{
	class Mesh;
	class Material;

	class MeshLoader : public Singleton<MeshLoader>
	{
	public:
		MeshLoader() {}
		Mesh* LoadMesh(const std::string& filePath, ID3D11Device* pDevice, Material* pMat);
	};
}

