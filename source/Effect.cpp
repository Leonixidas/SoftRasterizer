#include "pch.h"
#include "Effect.h"
#include <sstream>
#include <iostream>

Elite::Effect::Effect(ID3D11Device* pDevice, const std::wstring& file)
{
	m_pEffect = LoadEffect(pDevice, file);
	m_pCurrentTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pCurrentTechnique->IsValid())
	{
		std::wcout << L"Technique is not valid\n";
	}

	m_Techniques.push_back(m_pCurrentTechnique);
	m_CurrentTechniqueID = 0;

	m_pMatWVPVar = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWVPVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	m_pMatWorldVar = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	if (!m_pMatWorldVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	m_pMatViewInverseVar = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pMatViewInverseVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	m_pSRVDiffuseVar = m_pEffect->GetVariableByName("gDiffuse")->AsShaderResource();
	if (!m_pSRVDiffuseVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVNormalVar = m_pEffect->GetVariableByName("gNormal")->AsShaderResource();
	if (!m_pSRVNormalVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVSpecularVar = m_pEffect->GetVariableByName("gSpecular")->AsShaderResource();
	if (!m_pSRVSpecularVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";

	m_pSRVGlossinessVar = m_pEffect->GetVariableByName("gGlossiness")->AsShaderResource();
	if (!m_pSRVGlossinessVar->IsValid()) std::cout << "m_pSRVDiffuseVar is not valid\n";
}

Elite::Effect::~Effect()
{
	for (auto* t : m_Techniques)
		t->Release();
	m_pEffect->Release();
}

void Elite::Effect::LoadTechnique(const std::string& techniqueName)
{
	m_Techniques.push_back(m_pEffect->GetTechniqueByName(techniqueName.c_str()));
	if (m_pCurrentTechnique == nullptr) m_pCurrentTechnique = m_Techniques.back();
}

void Elite::Effect::SelectNextTechnique()
{
	if (m_CurrentTechniqueID + 1 >= m_Techniques.size())
	{
		m_CurrentTechniqueID = 0;
		m_pCurrentTechnique = m_Techniques[m_CurrentTechniqueID];
	}
	else
	{
		++m_CurrentTechniqueID;
		m_pCurrentTechnique = m_Techniques[m_CurrentTechniqueID];
	}
}

void Elite::Effect::SetWVPMat(const FMatrix4& mat)
{
	if(m_pMatWVPVar->IsValid())
		m_pMatWVPVar->SetMatrix(reinterpret_cast<const float*>(&mat));
}

void Elite::Effect::SetWorldMat(const FMatrix4& mat)
{
	if (m_pMatWorldVar->IsValid())
		m_pMatWorldVar->SetMatrix(reinterpret_cast<const float*>(&mat));
}

void Elite::Effect::SetViewInverseMat(const FMatrix4& mat)
{
	if (m_pMatViewInverseVar->IsValid())
		m_pMatViewInverseVar->SetMatrix(reinterpret_cast<const float*>(&mat));
}

void Elite::Effect::SetDiffuseMap(ID3D11ShaderResourceView* srv)
{
	if (m_pSRVDiffuseVar->IsValid())
		m_pSRVDiffuseVar->SetResource(srv);
}

void Elite::Effect::SetNormalMap(ID3D11ShaderResourceView* srv)
{
	if (m_pSRVNormalVar->IsValid())
		m_pSRVNormalVar->SetResource(srv);
}

void Elite::Effect::SetSpecularMap(ID3D11ShaderResourceView* srv)
{
	if (m_pSRVSpecularVar->IsValid())
		m_pSRVSpecularVar->SetResource(srv);
}

void Elite::Effect::SetGlossinessMap(ID3D11ShaderResourceView* srv)
{
	if (m_pSRVGlossinessVar->IsValid())
		m_pSRVGlossinessVar->SetResource(srv);
}

void Elite::Effect::SelectPreviousTechnique()
{
	if (m_CurrentTechniqueID - 1 > m_Techniques.size())
	{
		m_CurrentTechniqueID = m_Techniques.size() - 1;
		m_pCurrentTechnique = m_Techniques[m_CurrentTechniqueID];
	}
	else
	{
		--m_CurrentTechniqueID;
		m_pCurrentTechnique = m_Techniques[m_CurrentTechniqueID];
	}
}

ID3DX11Effect* Elite::Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring file)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(file.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << file;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}
