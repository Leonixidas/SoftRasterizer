#pragma once
#include "Material.h"

namespace Elite
{
	class Texture;
	
	class SpaceBikeMaterial : public Material
	{
	public:
		SpaceBikeMaterial(ID3D11Device*pDevice, const std::wstring& filepath);
		~SpaceBikeMaterial();
		virtual void LoadEffectVariables() override;
		virtual void UpdateEffectVariables() override;

		void SetDiffuseMap(const std::string& filePath, ID3D11Device* pDevice);
		void SetNormalMap(const std::string& filePath, ID3D11Device* pDevice);
		void SetSpecularMap(const std::string& filePath, ID3D11Device* pDevice);
		void SetGlossMap(const std::string& filePath, ID3D11Device* pDevice);

		virtual Texture* GetDiffuseTexture() override { return m_pDiffuseTexture; }
		virtual Texture* GetNormalTexture() override { return m_pNormalTexture; }
		virtual Texture* GetSpecularTexture() override { return m_pSpecularTexture; }
		virtual Texture* GetGlossTexture() override { return m_pGlossinessTexture; }

	private:
		Texture* m_pDiffuseTexture;
		ID3DX11EffectShaderResourceVariable* m_pSRVDiffuseVar;
		Texture* m_pNormalTexture;
		ID3DX11EffectShaderResourceVariable* m_pSRVNormalVar;
		Texture* m_pSpecularTexture;
		ID3DX11EffectShaderResourceVariable* m_pSRVSpecularVar;
		Texture* m_pGlossinessTexture;
		ID3DX11EffectShaderResourceVariable* m_pSRVGlossinessVar;
	};
}

