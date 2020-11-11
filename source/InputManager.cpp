#include "pch.h"
#include "InputManager.h"
#include "GameSettings.h"

bool Elite::InputManager::HandleInput()
{
	SDL_Event e;
	bool isQuiting{};
	m_TakeScreenShot = false;
	m_ReleasedKeys.clear();

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			isQuiting = true;
			break;
		case SDL_KEYUP:
			if (e.key.keysym.scancode == SDL_SCANCODE_X)
				m_TakeScreenShot = true;
			else if (e.key.keysym.scancode == SDL_SCANCODE_R)
				GameSettings::GetInstance().SwitchRasterizerMode();
			m_ReleasedKeys.push_back(e.key.keysym.scancode);
			break;
		}
	}

	m_pKeyboardState = SDL_GetKeyboardState(NULL);
	m_MouseState = SDL_GetRelativeMouseState(&m_MouseXValue, &m_MouseYValue);

	return isQuiting;
}

#pragma warning(push)
#pragma warning(disable : 26812)
bool Elite::InputManager::IsKeyPressed(const SDL_Scancode keyCode)
{
	return (m_pKeyboardState != 0 && m_pKeyboardState[SDL_Scancode(keyCode)]);
}

bool Elite::InputManager::IsKeyReleased(const SDL_Scancode code)
{
	return std::find(m_ReleasedKeys.begin(), m_ReleasedKeys.end(), SDL_Scancode(code)) != m_ReleasedKeys.end();
}
#pragma warning(pop)

bool Elite::InputManager::IsMouseButtonPressed(const MouseButton button)
{
	return (m_MouseState & SDL_BUTTON(int(button))) != 0;
}

void Elite::InputManager::GetMouseValues(float& x, float& y)
{
	x = float(m_MouseXValue);
	y = float(m_MouseYValue);
}
