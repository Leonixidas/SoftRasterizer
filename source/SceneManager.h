#pragma once
#include "Singleton.h"
#include <vector>
#include <string>

namespace Elite
{
	class Scene;

	class SceneManager final : public Singleton<SceneManager>
	{
		std::vector<Scene*> m_Scenes{};
		Scene* m_pCurrentScene = nullptr;
		size_t m_CurrentSceneID{};

	public:
		void AddScene(Scene* object);
		void RemoveScene(Scene* object);
		Scene* GetCurrentScene() { return m_pCurrentScene; }

		void NextScene();
		void PreviousScene();

		void Initialize(ID3D11Device* pDevice);

		void Update();

		void Render(ID3D11DeviceContext* pDeviceContext);

		void Close();
	};
}

