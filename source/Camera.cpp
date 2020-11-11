#include "pch.h"
#include "Camera.h"
#include "GameSettings.h"
#include "Enums.h"

Elite::Camera::Camera(const FPoint3& pos, const FPoint3& rot, float FOV, float aspectRatio)
	: m_Position(pos)
	, m_Rotation(rot)
	, m_AspectRatio(aspectRatio)
	, m_ProjectionMatrix()
	, m_ViewMatrix()
	, m_NearPlane(0.1f)
	, m_FarPlane(100.f)
{
	m_FOV = tanf((FOV * (float(E_PI) / 180.f)) / 2.f);
	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

const Elite::FMatrix4& Elite::Camera::GetViewMatrix()
{
	CalculateViewMatrix();
	return m_ViewMatrix;
}

const Elite::FMatrix4& Elite::Camera::GetProjectionMatrix()
{
	CalculateProjectionMatrix();
	return m_ProjectionMatrix;
}

void Elite::Camera::CalculateViewMatrix()
{
	FMatrix3 rotMat = MakeRotationZYX(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	FVector4 pos{ m_Position.x, m_Position.y, m_Position.z, 1.f };

	m_Forward = GetNormalized(rotMat * m_WorldForward);
	FVector3 forward = m_Forward;

	if (GameSettings::GetInstance().GetRasterizerMode() == RasterizerMode::DIRECTX)
	{
		pos.z *= -1.f;
		forward *= -1.f;
		//This is to get the same rotation when using the software rasterizer
		m_Forward.y *= -1.f;
		m_Forward.x *= -1.f;
	}

	m_Right = GetNormalized(Cross(m_WorldUp, forward));
	m_Up = GetNormalized(Cross(forward, m_Right));

	m_ViewMatrix = Inverse(FMatrix4{ m_Right, m_Up, forward, pos });

	if (GameSettings::GetInstance().GetRasterizerMode() == RasterizerMode::DIRECTX)
		m_Right.z *= -1.f;

}

void Elite::Camera::CalculateProjectionMatrix()
{
	FMatrix4 projMat{};

	projMat[0][0] = 1 / (m_AspectRatio * m_FOV);
	projMat[1][1] = 1 / m_FOV;
	projMat[2][2] = m_FarPlane / (m_NearPlane - m_FarPlane);
	projMat[2][3] = -1.f;
	projMat[3][2] = (m_NearPlane * m_FarPlane) / (m_NearPlane - m_FarPlane);

	if (GameSettings::GetInstance().GetRasterizerMode() == RasterizerMode::DIRECTX)
	{
		projMat[2][2] = m_FarPlane / (m_FarPlane - m_NearPlane);
		projMat[2][3] = 1.f;
		projMat[3][2] = -(m_NearPlane * m_FarPlane) / (m_FarPlane - m_NearPlane);
	}

	m_ProjectionMatrix = projMat;
}

void Elite::Camera::Translate(const FVector3& trans)
{
	m_Position += (trans.x * m_Right) + (trans.y * m_Up) + (trans.z * m_Forward);
}

void Elite::Camera::Rotate(const FVector3& rot)
{
	m_Rotation -= (rot * float((E_PI / 180.f)));
}
