#pragma once
#include "EMath.h"

namespace Elite
{
	class DirectionalLight
	{
	public:
		DirectionalLight() {}
		~DirectionalLight() = default;

		const FVector3& GetLightDirection() { return m_Direction; }

		float GetLightIntensity() { return m_Intensity; }

		void SetLightDirection(const FVector3& dir) { m_Direction = dir; }

		void SetLightIntensity(float intensity) { m_Intensity = intensity; }

	private:
		FVector3 m_Direction = { 0.577f,-0.577f,-0.577f };
		float m_Intensity = 2.f;
	};
}

