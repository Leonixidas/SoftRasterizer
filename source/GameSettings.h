#pragma once
#include "Singleton.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "Enums.h"

namespace Elite
{
	class GameSettings : public Singleton<GameSettings>
	{
	public:
		GameSettings() {};
		~GameSettings() = default;

		Camera* GetCamera() { return m_pCurrentCamera; }
		void SetCamera(Camera* cam) { m_pCurrentCamera = cam; }

		const RasterizerMode& GetRasterizerMode() { return m_Mode; }
		void SwitchRasterizerMode()
		{
			if (m_Mode == RasterizerMode::DIRECTX)
			{
				m_Mode = RasterizerMode::SOFT_RAST;
				m_DirLight.SetLightDirection({ 0.577f,-0.577f,-0.577f });
			}
			else
			{
				m_Mode = RasterizerMode::DIRECTX;
				m_DirLight.SetLightDirection({ 0.577f,-0.577f,0.577f });
			}
		}

		const FPoint2& GetWindowDimensions() { return m_WindowDimensions; }
		void SetWindowDimensions(const FPoint2& dim) { m_WindowDimensions = dim; }

		void SetElapsedSec(float elapsed) { m_DeltaTime = elapsed; }
		float GetElapsedSec() { return m_DeltaTime; }

		const DirectionalLight& GetDirectionalLight() { return m_DirLight; }

	private:
		Camera* m_pCurrentCamera = nullptr;
		DirectionalLight m_DirLight{};
		RasterizerMode m_Mode = RasterizerMode::DIRECTX;
		FPoint2 m_WindowDimensions{};
		float m_DeltaTime{};
	};

}

