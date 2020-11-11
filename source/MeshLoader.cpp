#include "pch.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include <fstream>
#include "Structs.h"

Elite::Mesh* Elite::MeshLoader::LoadMesh(const std::string& filePath, ID3D11Device* pDevice, Material* pMat)
{
	std::ifstream input{ filePath };
	if (!input) return nullptr;
	std::string line{};
	char first{};

	std::vector<FPoint4> vertices{};
	std::vector<FVector3> normals{};
	std::vector<FVector2> texcoords{};

	std::vector<Vertex> vertexData{};
	std::vector<uint32_t> indices;


	while (!input.eof())
	{
		input >> first;

		switch (first)
		{
		case 'v':
		{
			input.read(&first, 1);

			if (first == ' ')
			{
				FPoint4 vertex{};
				input >> vertex.x >> vertex.y >> vertex.z;
				vertex.z *= -1;
				vertex.w = 1.f;
				vertices.push_back(vertex);
			}
			else if (first == 'n')
			{
				FVector3 normal{};
				input >> normal.x >> normal.y >> normal.z;
				normal.z *= -1;
				normals.push_back(normal);
			}
			else if (first == 't')
			{
				FVector2 uv{};
				input >> uv.x >> uv.y;
				uv.y = 1 - uv.y;
				texcoords.push_back(uv);
			}
		}
		break;
		case 'f':
		{
			size_t v{}, vn{}, vt{};
			Vertex vertex{};

			for (int i = 0; i < 3; ++i)
			{
				input >> v;
				input.read(&first, 1);
				input >> vt;
				input.read(&first, 1);
				input >> vn;

				vertex.pos = vertices[v - 1];
				vertex.normal = normals[vn - 1];
				vertex.uv = texcoords[vt - 1];

				auto iter = std::find_if(vertexData.begin(), vertexData.end(),
					[&vertex](Vertex& v) { return v.pos == vertex.pos && v.uv == vertex.uv; });

				if (iter == vertexData.end())
				{
					vertexData.push_back(vertex);
					indices.push_back(uint32_t(vertexData.size() - 1));
				}
				else
				{
					indices.push_back(uint32_t(std::distance(vertexData.begin(), iter)));
				}
			}
		}
		break;
		default:
			std::getline(input, line);
			break;
		}
	}

	input.close();

	//TANGENT CALCULATION
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		size_t index0 = indices[i];
		size_t index1 = indices[i + 1];
		size_t index2 = indices[i + 2];

		const FPoint4& p0 = vertexData[index0].pos;
		const FPoint4& p1 = vertexData[index1].pos;
		const FPoint4& p2 = vertexData[index2].pos;

		const FVector2& uv0 = vertexData[index0].uv;
		const FVector2& uv1 = vertexData[index1].uv;
		const FVector2& uv2 = vertexData[index2].uv;

		const FVector4 edge0 = p1 - p0;
		const FVector4 edge1 = p2 - p0;
		const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Cross(diffX, diffY);

		FVector4 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertexData[index0].tangent += tangent.xyz;
		vertexData[index1].tangent += tangent.xyz;
		vertexData[index2].tangent += tangent.xyz;
	}

	for (auto& v : vertexData)
	{
		v.tangent = GetNormalized(Reject(v.tangent, v.normal));
		v.tangent.z *= -1.f;
	}

	return new Mesh{ pDevice,vertexData,indices, {}, pMat };
}
