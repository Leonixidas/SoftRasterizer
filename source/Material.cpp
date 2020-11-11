#include "pch.h"
#include "Material.h"
#include <sstream>


Elite::Material::Material(ID3D11Device* pDevice, const std::wstring& filePath)
{
	m_pEffect = LoadEffect(pDevice, filePath);
	m_pCurrentTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pCurrentTechnique->IsValid())
	{
		std::wcout << L"Technique is not valid\n";
	}

	m_Techniques.push_back(m_pCurrentTechnique);
	m_CurrentTechniqueID = 0;
}

Elite::Material::~Material()
{
	for (auto* t : m_Techniques)
		t->Release();
	m_pEffect->Release();
}

void Elite::Material::Initialize()
{
	m_pMatWVPVar = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWVPVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	m_pMatWorldVar = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	if (!m_pMatWorldVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	m_pMatViewInverseVar = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pMatViewInverseVar->IsValid()) std::cout << "m_pMatWVPVar is not valid\n";

	LoadEffectVariables();
}

void Elite::Material::SetEffectVariables(const FMatrix4& projMat, const FMatrix4& viewMat, const FMatrix4& worldMat)
{
	if (m_pMatWorldVar)
		m_pMatWorldVar->SetMatrix(reinterpret_cast<const float*>(&worldMat));

	if (m_pMatWVPVar)
	{
		auto wvp = projMat * viewMat * worldMat;
		m_pMatWVPVar->SetMatrix(reinterpret_cast<const float*>(&(wvp)));
	}

	if (m_pMatViewInverseVar)
	{
		FMatrix4 viewInverse = Inverse(viewMat);
		m_pMatViewInverseVar->SetMatrix(reinterpret_cast<float*>(&viewInverse));
	}

	UpdateEffectVariables();
}

void Elite::Material::LoadTechnique(const std::string& techniqueName)
{
	m_Techniques.push_back(m_pEffect->GetTechniqueByName(techniqueName.c_str()));
	if (m_pCurrentTechnique == nullptr) m_pCurrentTechnique = m_Techniques.back();
}

void Elite::Material::SelectNextTechnique()
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

void Elite::Material::SelectPreviousTechnique()
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

ID3DX11Effect* Elite::Material::LoadEffect(ID3D11Device* pDevice, const std::wstring file)
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