#pragma once
#include "Material.h"

namespace Elite
{
	class Texture;

	class FlameMaterial : public Material
	{
	public:
		FlameMaterial(ID3D11Device* pDevice, const std::wstring& filePath);
		~FlameMaterial();

		virtual void LoadEffectVariables() override;
		virtual void UpdateEffectVariables() override;

		void SetDiffuseMap(const std::string& filePath, ID3D11Device* pDevice);

		void SwitchUseTransparency() { m_UseTransparency = !m_UseTransparency; }

		virtual Texture* GetDiffuseTexture() override { return m_pDiffuseTexture; }
		virtual Texture* GetNormalTexture() override { return nullptr; }
		virtual Texture* GetSpecularTexture() override { return nullptr; }
		virtual Texture* GetGlossTexture() override { return nullptr; }

	private:
		Texture* m_pDiffuseTexture;
		ID3DX11EffectShaderResourceVariable* m_pSRVDiffuseVar;
		bool m_UseTransparency = true;
		ID3DX11EffectScalarVariable* m_pSRVUseTransparency;
	};
}

