#pragma once
#include <vector>

namespace Elite
{
	class Texture;

	class Material
	{
	public:
		explicit Material(ID3D11Device* pDevice, const std::wstring& filePath);
		virtual ~Material();
		void Initialize();
		void SetEffectVariables(const FMatrix4& projMat, const FMatrix4& viewMat, const FMatrix4& worldMat);
		ID3DX11EffectTechnique* GetCurrentTechnique() { return m_pCurrentTechnique; }
		void LoadTechnique(const std::string& techniqueName);
		void SelectPreviousTechnique();
		void SelectNextTechnique();
		virtual void LoadEffectVariables() = 0;
		virtual void UpdateEffectVariables() = 0;

		virtual Texture* GetDiffuseTexture() = 0;
		virtual Texture* GetNormalTexture() = 0;
		virtual Texture* GetSpecularTexture() = 0;
		virtual Texture* GetGlossTexture() = 0;

	protected:
		std::vector<ID3DX11EffectTechnique*> m_Techniques;
		ID3DX11Effect* m_pEffect;
		ID3DX11EffectTechnique* m_pCurrentTechnique;
		ID3DX11EffectMatrixVariable* m_pMatWVPVar;
		ID3DX11EffectMatrixVariable* m_pMatWorldVar;
		ID3DX11EffectMatrixVariable* m_pMatViewInverseVar;
		size_t m_CurrentTechniqueID;
	private:
		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring file);
	};
}

