#pragma once
#include "Scene.h"
#include <d3dx11effect.h>

namespace Elite
{
	class FlameMaterial;

	class SpaceBikeScene : public Scene
	{
	public:
		SpaceBikeScene(const std::string& name);
		~SpaceBikeScene() = default;

		virtual void Initialize(ID3D11Device* pDevice) override;
		virtual void Update() override;

	private:
		Mesh* m_pMesh;
		Mesh* m_pFlames;
		FlameMaterial* m_pFlameMat;
	};
}

