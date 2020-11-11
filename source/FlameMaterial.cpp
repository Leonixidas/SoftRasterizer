#include "pch.h"
#include "FlameMaterial.h"
#include "Texture.h"

Elite::FlameMaterial::FlameMaterial(ID3D11Device* pDevice, const std::wstring& filePath)
	: Material(pDevice, filePath)
	, m_pDiffuseTexture(nullptr)
	, m_pSRVDiffuseVar(nullptr)
	, m_pSRVUseTransparency(nullptr)
{
}

Elite::FlameMaterial::~FlameMaterial()
{
	if (m_pDiffuseTexture != nullptr) delete m_pDiffuseTexture;
}

void Elite::FlameMaterial::LoadEffectVariables()
{
	m_pSRVDiffuseVar = m_pEffect->GetVariableByName("gDiffuse")->AsShaderResource();
	if (!m_pSRVDiffuseVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVUseTransparency = m_pEffect->GetVariableByName("gUseTransparency")->AsScalar();
	if (!m_pSRVUseTransparency->IsValid()) std::cout << "m_pSRVUseTransparency is not valid\n";
	
}

void Elite::FlameMaterial::UpdateEffectVariables()
{
	if (m_pSRVDiffuseVar->IsValid())
		m_pSRVDiffuseVar->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	if (m_pSRVUseTransparency->IsValid())
		m_pSRVUseTransparency->SetBool(m_UseTransparency);
}

void Elite::FlameMaterial::SetDiffuseMap(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pDiffuseTexture = new Texture();
	m_pDiffuseTexture->LoadTexture(filePath, pDevice);
}