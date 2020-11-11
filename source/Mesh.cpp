#include "pch.h"
#include "Mesh.h"
#include "Structs.h"
#include "GameSettings.h"
#include "Enums.h"
#include "Material.h"

Elite::Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const FPoint3& pos, Material* pMat)
{
	m_pMaterial = pMat;
	m_pInputLayout = nullptr;
	m_pIndexBuffer = nullptr;
	m_pVertexBuffer = nullptr;
	m_AmountIndices = 0;
	m_Position = pos;

	m_Vertices = std::move(vertices);
	m_Indices = std::move(indices);

	CalculateWorldMatrix();


	//Create Vertex layout
	HRESULT result = S_OK;
	static const uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 16;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TANGENT";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 28;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 40;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create input layout
	D3DX11_PASS_DESC passDesc;
	pMat->GetCurrentTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result)) return;

	//Create Vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * (uint32_t)m_Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_Vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	if (FAILED(result)) return;

	//Create index buffer
	m_AmountIndices = (uint32_t)m_Indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	initData.pSysMem = m_Indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	if (FAILED(result)) return;
}

Elite::Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexBuffer->Release();
	m_pInputLayout->Release();
	if (m_pMaterial != nullptr) delete m_pMaterial;
}

void Elite::Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	//WVP
	FMatrix4 proj = GameSettings::GetInstance().GetCamera()->GetProjectionMatrix();
	FMatrix4 view = GameSettings::GetInstance().GetCamera()->GetViewMatrix();

	m_pMaterial->SetEffectVariables(proj, view, m_WorldMatrix);
	
	//Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the input layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pMaterial->GetCurrentTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pMaterial->GetCurrentTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

Elite::FMatrix4 Elite::Mesh::GetWorldMatrix()
{
	CalculateWorldMatrix();
	return m_WorldMatrix;
}

void Elite::Mesh::Translate(const FVector3& pos)
{
	m_Position += pos;
	CalculateWorldMatrix();
}

void Elite::Mesh::Rotate(const FVector3& rot)
{
	m_Rotation += rot * (float(E_PI) / 180.f);
	CalculateWorldMatrix();
	
}

void Elite::Mesh::CalculateWorldMatrix()
{
	FMatrix4 rotMat = MakeRotationZYX(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	FMatrix4 trans = MakeTranslation(FVector3{ m_Position.x, m_Position.y, m_Position.z });

	m_WorldMatrix = rotMat * trans;
}
