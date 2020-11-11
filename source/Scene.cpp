#include "pch.h"
#include "Scene.h"
#include "Mesh.h"
#include "GameSettings.h"

Elite::Scene::Scene(const std::string& sceneName)
	: m_SceneName(sceneName)
	, m_IsInitialized(false)
	, m_CurrentCamIndex(0)
{
}

Elite::Scene::~Scene()
{
	for (auto mesh : m_Meshes)
		delete mesh;

	for (auto cam : m_Cameras)
		delete cam;

	m_Meshes.clear();
	m_Cameras.clear();
}

void Elite::Scene::RootInitialize(ID3D11Device* pDevice)
{
	Initialize(pDevice);
	m_IsInitialized = true;
}

void Elite::Scene::RootUpdate()
{
	Update();
}

void Elite::Scene::Render(ID3D11DeviceContext* pDeviceContext)
{
	for (auto mesh : m_Meshes)
	{
		mesh->Render(pDeviceContext);
	}
}

void Elite::Scene::AddMesh(Mesh* mesh)
{
	auto iter = std::find(m_Meshes.begin(), m_Meshes.end(), mesh);
	if (iter == m_Meshes.end()) m_Meshes.push_back(mesh);
}

void Elite::Scene::AddCamera(Camera* cam)
{
	auto iter = std::find(m_Cameras.begin(), m_Cameras.end(), cam);
	if (iter == m_Cameras.end()) m_Cameras.push_back(cam);

	if (m_Cameras.size() == 1) GameSettings::GetInstance().SetCamera(cam);
}
