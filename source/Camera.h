#pragma once
#include "EMath.h"

namespace Elite
{
	class Camera
	{
	public:
		Camera(const FPoint3& pos, const FPoint3& rot, float FOV, float aspectRatio);
		~Camera() = default;

		const FMatrix4& GetViewMatrix(); // default is right handed coordinate system
		const FMatrix4& GetProjectionMatrix();

		void CalculateViewMatrix();
		void CalculateProjectionMatrix();

		void Translate(const FVector3& trans);
		void Rotate(const FVector3& rot);


	private:
		FMatrix4 m_ViewMatrix;
		FMatrix4 m_ProjectionMatrix;
		FPoint3 m_Position;
		FVector3 m_Rotation;
		FVector3 m_Forward, m_Right, m_Up;
		const FVector3 m_WorldUp{ 0.f,1.f,0.f };
		const FVector3 m_WorldForward{ 0.f,0.f,-1.f }; // this is right handed coordinate system
		float m_FOV;
		float m_AspectRatio;
		float m_NearPlane;
		float m_FarPlane;
	};
}

