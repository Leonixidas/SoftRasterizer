#pragma once
#include "SDL.h"
#include "Singleton.h"
#include <vector>
#include "Enums.h"

namespace Elite
{
	class InputManager : public Singleton<InputManager>
	{
	public:
		bool HandleInput();
		bool IsKeyPressed(const SDL_Scancode keyCode);
		bool IsKeyReleased(const SDL_Scancode code);

		bool IsMouseButtonPressed(const MouseButton button);

		void GetMouseValues(float& x, float& y);

		bool IsScreenShotTaken() { return m_TakeScreenShot; }

		bool AreShadowsEnabled() { return m_ShadowsEnabled; }

	private:
		const Uint8* m_pKeyboardState = nullptr;
		Uint8 m_MouseState{};
		std::vector<SDL_Scancode> m_ReleasedKeys;
		int m_MouseXValue{}, m_MouseYValue{};
		bool m_TakeScreenShot{}, m_ShadowsEnabled{true};
	};
}

