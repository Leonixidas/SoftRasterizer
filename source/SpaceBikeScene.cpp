#include "pch.h"
#include "SpaceBikeScene.h"
#include "Mesh.h"
#include "InputManager.h"
#include "GameSettings.h"
#include "MeshLoader.h"
#include "Camera.h"
#include "Structs.h"
#include "SpaceBikeMaterial.h"
#include "FlameMaterial.h"

Elite::SpaceBikeScene::SpaceBikeScene(const std::string& name)
	: Scene(name)
	, m_pMesh(nullptr)
	, m_pFlames(nullptr)
	, m_pFlameMat(nullptr)
{
}

void Elite::SpaceBikeScene::Initialize(ID3D11Device* pDevice)
{
	FPoint2 screenDim = GameSettings::GetInstance().GetWindowDimensions();
	AddCamera(new Camera{ {0,0,75},{0,0,0},45.f, screenDim.x / screenDim.y });

	SpaceBikeMaterial* pMat = new SpaceBikeMaterial(pDevice, L"Resources/PosCol3D.fx");
	pMat->SetDiffuseMap("Resources/vehicle_diffuse.png", pDevice);
	pMat->SetNormalMap("Resources/vehicle_normal.png", pDevice);
	pMat->SetSpecularMap("Resources/vehicle_specular.png", pDevice);
	pMat->SetGlossMap("Resources/vehicle_gloss.png", pDevice);
	pMat->LoadTechnique("LinearTechnique");
	pMat->LoadTechnique("AnisotropicTechnique");
	pMat->Initialize();

	m_pMesh = MeshLoader::GetInstance().LoadMesh("Resources/vehicle.obj", pDevice, pMat);
	m_pMesh->SetMaterial(pMat);
	m_pMesh->Translate({ 0.f,0.f,0.f });
	AddMesh(m_pMesh);

	m_pFlameMat = new FlameMaterial(pDevice, L"Resources/PosCol3DPartialCoverage.fx");
	m_pFlameMat->SetDiffuseMap("Resources/fireFX_diffuse.png", pDevice);
	m_pFlameMat->LoadTechnique("LinearTechnique");
	m_pFlameMat->LoadTechnique("AnisotropicTechnique");
	m_pFlameMat->Initialize();

	m_pFlames = MeshLoader::GetInstance().LoadMesh("Resources/fireFX.obj", pDevice, m_pFlameMat);
	m_pFlames->SetMaterial(m_pFlameMat);
	AddMesh(m_pFlames);
}

void Elite::SpaceBikeScene::Update()
{
	GameSettings& gs = GameSettings::GetInstance();
	float deltaTime = gs.GetElapsedSec();
	float cameraSpeed = 5.f;
	InputManager& input = InputManager::GetInstance();
	FVector3 translation{}, rotation{};
	bool transformed = false;

	if (input.IsKeyPressed(SDL_SCANCODE_D))
	{
		translation.x += cameraSpeed * deltaTime;
		transformed = true;
	}
	if (input.IsKeyPressed(SDL_SCANCODE_A))
	{
		translation.x -= cameraSpeed * deltaTime;
		transformed = true;
	}
	if (input.IsKeyPressed(SDL_SCANCODE_W))
	{
		translation.z += cameraSpeed * deltaTime;
		transformed = true;
	}
	if (input.IsKeyPressed(SDL_SCANCODE_S))
	{
		translation.z -= cameraSpeed * deltaTime;
		transformed = true;
	}

	if (input.IsKeyReleased(SDL_SCANCODE_R)) m_pFlames->SwitchIsEnabled();
	if (input.IsKeyReleased(SDL_SCANCODE_F))
	{
		m_pFlames->GetMaterial()->SelectNextTechnique();
		m_pMesh->GetMaterial()->SelectNextTechnique();
	}
	if (input.IsKeyReleased(SDL_SCANCODE_T)) m_pFlameMat->SwitchUseTransparency();

	if (input.IsMouseButtonPressed(MouseButton::MOUSE_LEFT) && input.IsMouseButtonPressed(MouseButton::MOUSE_RIGHT))
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		float x{}, y{};
		input.GetMouseValues(x, y);

		translation.y += -y * cameraSpeed * deltaTime;
		transformed = true;
	}
	else if (input.IsMouseButtonPressed(MouseButton::MOUSE_LEFT))
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		float x{}, y{};
		input.GetMouseValues(x, y);

		rotation.y += -x * cameraSpeed * deltaTime;
		translation.z += -y * cameraSpeed * deltaTime;
		transformed = true;
	}
	else if (input.IsMouseButtonPressed(MouseButton::MOUSE_RIGHT))
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		float x{}, y{};
		input.GetMouseValues(x, y);

		rotation.x += -y * cameraSpeed * deltaTime;
		rotation.y += -x * cameraSpeed * deltaTime;
		transformed = true;
	}
	else SDL_SetRelativeMouseMode(SDL_FALSE);

	if (transformed)
	{
		gs.GetCamera()->Translate(translation);
		gs.GetCamera()->Rotate(rotation);
	}
}
