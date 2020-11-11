#pragma once
#include <vector>
#include <string>

namespace Elite
{
	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& file);
		~Effect();

		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring file);
		void LoadTechnique(const std::string& techniqueName);
		void SelectPreviousTechnique();
		void SelectNextTechnique();

		void SetWVPMat(const FMatrix4& mat);
		void SetWorldMat(const FMatrix4& mat);
		void SetViewInverseMat(const FMatrix4& mat);

		void SetDiffuseMap(ID3D11ShaderResourceView* srv);
		void SetNormalMap(ID3D11ShaderResourceView* srv);
		void SetSpecularMap(ID3D11ShaderResourceView* srv);
		void SetGlossinessMap(ID3D11ShaderResourceView* srv);

		ID3DX11EffectTechnique* GetCurrentTechnique() { return m_pCurrentTechnique; }
		ID3DX11Effect* GetEffect() { return m_pEffect; }
	
	private:
		std::vector<ID3DX11EffectTechnique*> m_Techniques;
		ID3DX11Effect* m_pEffect;
		ID3DX11EffectTechnique* m_pCurrentTechnique;
		ID3DX11EffectMatrixVariable* m_pMatWVPVar;
		ID3DX11EffectMatrixVariable* m_pMatWorldVar;
		ID3DX11EffectMatrixVariable* m_pMatViewInverseVar;
		ID3DX11EffectShaderResourceVariable* m_pSRVDiffuseVar;
		ID3DX11EffectShaderResourceVariable* m_pSRVNormalVar;
		ID3DX11EffectShaderResourceVariable* m_pSRVSpecularVar;
		ID3DX11EffectShaderResourceVariable* m_pSRVGlossinessVar;
		size_t m_CurrentTechniqueID;
	};
}

