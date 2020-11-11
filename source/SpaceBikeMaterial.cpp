#include "pch.h"
#include "SpaceBikeMaterial.h"
#include "Texture.h"

Elite::SpaceBikeMaterial::SpaceBikeMaterial(ID3D11Device* pDevice, const std::wstring& filepath)
	: Material(pDevice, filepath)
	, m_pDiffuseTexture(nullptr)
	, m_pNormalTexture(nullptr)
	, m_pSpecularTexture(nullptr)
	, m_pGlossinessTexture(nullptr)
	, m_pSRVDiffuseVar(nullptr)
	, m_pSRVNormalVar(nullptr)
	, m_pSRVSpecularVar(nullptr)
	, m_pSRVGlossinessVar(nullptr)
{
}

Elite::SpaceBikeMaterial::~SpaceBikeMaterial()
{
	delete m_pDiffuseTexture;
	delete m_pNormalTexture;
	delete m_pSpecularTexture;
	delete m_pGlossinessTexture;
}

void Elite::SpaceBikeMaterial::LoadEffectVariables()
{
	m_pSRVDiffuseVar = m_pEffect->GetVariableByName("gDiffuse")->AsShaderResource();
	if (!m_pSRVDiffuseVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVNormalVar = m_pEffect->GetVariableByName("gNormal")->AsShaderResource();
	if (!m_pSRVNormalVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVSpecularVar = m_pEffect->GetVariableByName("gSpecular")->AsShaderResource();
	if (!m_pSRVSpecularVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVGlossinessVar = m_pEffect->GetVariableByName("gGlossiness")->AsShaderResource();
	if (!m_pSRVGlossinessVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";
}

void Elite::SpaceBikeMaterial::UpdateEffectVariables()
{
	if (m_pSRVDiffuseVar->IsValid())
		m_pSRVDiffuseVar->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	if (m_pSRVNormalVar->IsValid())
		m_pSRVNormalVar->SetResource(m_pNormalTexture->GetShaderResourceView());
	if (m_pSRVSpecularVar->IsValid())
		m_pSRVSpecularVar->SetResource(m_pSpecularTexture->GetShaderResourceView());
	if (m_pSRVGlossinessVar->IsValid())
		m_pSRVGlossinessVar->SetResource(m_pGlossinessTexture->GetShaderResourceView());
}

void Elite::SpaceBikeMaterial::SetDiffuseMap(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pDiffuseTexture = new Texture();
	m_pDiffuseTexture->LoadTexture(filePath, pDevice);
}

void Elite::SpaceBikeMaterial::SetNormalMap(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pNormalTexture = new Texture();
	m_pNormalTexture->LoadTexture(filePath, pDevice);
}

void Elite::SpaceBikeMaterial::SetSpecularMap(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pSpecularTexture = new Texture();
	m_pSpecularTexture->LoadTexture(filePath, pDevice);
}

void Elite::SpaceBikeMaterial::SetGlossMap(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pGlossinessTexture = new Texture();
	m_pGlossinessTexture->LoadTexture(filePath, pDevice);
}
