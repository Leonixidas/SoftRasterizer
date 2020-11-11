#include "pch.h"
//#undef main

//Standard includes
#include <iostream>
#include "vld.h"

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "GameSettings.h"
#include "SpaceBikeScene.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Merged Rasterizer - Leander De Hertogh",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;
	std::cout << "**********************************\n";
	std::cout << "\t\tCamera\n";
	std::cout << "**********************************\n\n";
	std::cout << "Move around: WASD-keys\n";
	std::cout << "Move up or down: hold left and right mouse button and move up or down\n";
	std::cout << "Rotate Camera: Hold right mouse button and move around\n";
	std::cout << "Move and rotate: Hold left mouse button and move around\n\n";

	std::cout << "**********************************\n";
	std::cout << "\t\Mode switching\n";
	std::cout << "**********************************\n\n";
	std::cout << "Switch rasterizer mode: 'R'\n";
	std::cout << "Transparency ON/OFF: 'T'\n";
	std::cout << "Sample Mode: 'F'\n\n";

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };
	Elite::InputManager& input = Elite::InputManager::GetInstance();
	Elite::SceneManager& sceneManager = Elite::SceneManager::GetInstance();
	Elite::GameSettings& gs = Elite::GameSettings::GetInstance();
	sceneManager.AddScene(new Elite::SpaceBikeScene{ "SpaceBikeScene" });

	sceneManager.Initialize(pRenderer->GetDevice());

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	while (isLooping)
	{
		//--------- Get input events ---------
		isLooping = !input.HandleInput();

		//--------- Update ----------
		sceneManager.Update();

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}
		gs.SetElapsedSec(pTimer->GetElapsed());

	}
	pTimer->Stop();
	sceneManager.Close();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}