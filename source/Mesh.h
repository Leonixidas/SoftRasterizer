#pragma once
#include <vector>

namespace Elite
{
	struct Vertex;
	class Material;

	class Mesh
	{
	public:
		Mesh(ID3D11Device* pDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const FPoint3& pos, Material* pMat);
		~Mesh();

		void Render(ID3D11DeviceContext* pDeviceContext);

		FMatrix4 GetWorldMatrix();

		void SwitchIsEnabled() { m_IsEnabled = !m_IsEnabled; }

		bool GetIsEnabled() { return m_IsEnabled; }

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
		uint32_t GetVerticesAmount() { return m_AmountIndices; }

		const std::vector<Vertex>& GetVertices() { return m_Vertices;}
		const std::vector<uint32_t>& GetIndices() { return m_Indices; }

		void Translate(const FVector3& pos);

		void Rotate(const FVector3& rot);

		void SetMaterial(Material* pMat) { m_pMaterial = pMat; }
		Material* GetMaterial() { return m_pMaterial; }

	private:

		void CalculateWorldMatrix();
		FPoint3 m_Position;
		FVector3 m_Rotation;
		FMatrix4 m_WorldMatrix;

		//DirectX
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11InputLayout* m_pInputLayout;

		//SoftRaster
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		//Textures
		Material* m_pMaterial;

		uint32_t m_AmountIndices;

		bool m_IsEnabled = true;
	};
}

