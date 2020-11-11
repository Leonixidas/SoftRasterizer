#pragma once
#include "EMath.h"
#include "Mesh.h"

namespace Elite 
{
	struct Vertex
	{
		FPoint4 pos = {};
		FVector3 normal = {};
		FVector3 tangent = {};
		FVector2 uv = {};
	};

	struct VS_OUTPUT
	{
		FPoint4 pos = {};
		FPoint4 worldPos = {};
		FVector3 normal = {};
		FVector3 tangent = {};
		FVector2 uv = {};
		Mesh* mesh = nullptr;
	};

	struct VS_WORLDPOS
	{
		FVector4 v0{}, v1{}, v2{};
	};

	struct HitInfo
	{
		RGBColor color = {};
		FPoint2 screenPos = {};
		float depthValue = {};
	};
}