#include "pch.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Scene.h"

void Elite::SceneManager::AddScene(Scene* scene)
{
	if (std::find(m_Scenes.begin(), m_Scenes.end(), scene) == m_Scenes.end())
		m_Scenes.push_back(scene);
}

void Elite::SceneManager::RemoveScene(Scene* scene)
{
	auto iter = std::find(m_Scenes.begin(), m_Scenes.end(), scene);
	if (iter != m_Scenes.end())
		m_Scenes.erase(iter);
}

void Elite::SceneManager::NextScene()
{
	if (m_CurrentSceneID + 1 == m_Scenes.size())
		m_CurrentSceneID = 0;
	else ++m_CurrentSceneID;

	m_pCurrentScene = m_Scenes[m_CurrentSceneID];
}

void Elite::SceneManager::PreviousScene()
{
	if (m_CurrentSceneID - 1 == -1)
		m_CurrentSceneID = m_Scenes.size() - 1;
	else --m_CurrentSceneID;

	m_pCurrentScene = m_Scenes[m_CurrentSceneID];
}

void Elite::SceneManager::Initialize(ID3D11Device* pDevice)
{
	if (m_Scenes.size() == 0) return;

	for (Scene* s : m_Scenes)
	{
		s->RootInitialize(pDevice);
	}

	m_pCurrentScene = m_Scenes[0];
	m_CurrentSceneID = 0;
}

void Elite::SceneManager::Update()
{
	if (InputManager::GetInstance().IsKeyReleased(SDL_SCANCODE_1)) PreviousScene();
	if (InputManager::GetInstance().IsKeyReleased(SDL_SCANCODE_2)) NextScene();

	if(m_pCurrentScene != nullptr)
		m_pCurrentScene->RootUpdate();
}

void Elite::SceneManager::Render(ID3D11DeviceContext* pDeviceContext)
{
	m_pCurrentScene->Render(pDeviceContext);
}

void Elite::SceneManager::Close()
{
	for (auto scene : m_Scenes)
	{
		delete scene;
		scene = nullptr;
	}
}
