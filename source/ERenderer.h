/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Structs.h"

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Texture;
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render();

		ID3D11Device* GetDevice() { return m_pDevice; }

	private:

		//Shading
		RGBColor PixelShader(VS_OUTPUT& input);
		FVector3 CalculateNewNormal(const FVector3& tangent, const FVector3& normal, const FVector2& uv, Texture* normalMap);
		RGBColor CalculateSpecularPhong(const FVector3& viewDir, const FVector3& normal, const FVector2& uv, Texture* SpecularMap, Texture* glossMap);
		RGBColor CalculateDiffuse(const FVector3& normal, const FVector2& uv, Texture* diffuseMap);

		//Rasterizer
		bool PointInTriangle(HitInfo& hit, const Vertex& v0, const Vertex& v1, const Vertex& v2, VS_OUTPUT& out, const VS_WORLDPOS& worldPos);
		void VertexTransformationFunc(Vertex& v0, Vertex& v1, Vertex& v2, VS_WORLDPOS& worldPos, const FMatrix4& projMat, const FMatrix4& viewMat, const FMatrix4& worldMat);
		FMatrix2 GetBoundingBox(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		bool IsInFrustum(const FMatrix2& bBox);
		bool IsInScreenBorders(const FMatrix2& bBox);
		bool IsInViewRange(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void TransformToRasterSpace(Vertex& v0, Vertex& v1, Vertex& v2, FMatrix2& bBox);


		void InitializeSDL();
		HRESULT InitializeDirectX();

		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;

		//SoftRast
		SDL_Surface* m_pFrontBuffer;
		SDL_Surface* m_pBackBuffer;
		uint32_t* m_pBackBufferPixels;
		std::vector<float> m_DepthBuffer;

		//DirectX
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		bool m_IsInitialized;
	};
}

#endif