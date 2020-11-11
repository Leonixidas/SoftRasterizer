#include "pch.h"

//Project includes
#include "ERenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Mesh.h"
#include "Camera.h"
#include "Effect.h"
#include "Texture.h"
#include "GameSettings.h"
#include "Material.h"


Elite::Renderer::Renderer(SDL_Window* pWindow)
	: m_pWindow( pWindow )
	, m_Width()
	, m_Height()
	, m_IsInitialized( false )
	, m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pDXGIFactory(nullptr)
	, m_pSwapChain(nullptr)
	, m_pRenderTargetBuffer(nullptr)
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilBuffer(nullptr)
	, m_pDepthStencilView(nullptr)
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);

	GameSettings::GetInstance().SetWindowDimensions(FPoint2{ float(m_Width), float(m_Height) });

	//Initialize SDL
	InitializeSDL();

	//Initialize DirectX pipeline
	InitializeDirectX();
	m_IsInitialized = true;
	std::wcout << L"DirectX is ready\n";

}

Elite::Renderer::~Renderer()
{
	if (m_pRenderTargetView) m_pRenderTargetView->Release();
	if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();

	if (m_pDepthStencilView) m_pDepthStencilView->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();

	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pDXGIFactory) m_pDXGIFactory->Release();

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
	}

}

void Elite::Renderer::Render()
{
	//-------------------------------
	//Variables needed for both modes
	//-------------------------------
	SceneManager& sceneM = SceneManager::GetInstance();
	GameSettings& gameS = GameSettings::GetInstance();

	//Scene variables
	const std::vector<Mesh*>& meshes = sceneM.GetCurrentScene()->GetMeshes();
	Camera* cam = gameS.GetCamera();
	
	//Matrices
	FMatrix4 projMat{ cam->GetProjectionMatrix() }, viewMat{ cam->GetViewMatrix() }, worldMat{};

	//Clear window
	if (gameS.GetRasterizerMode() == RasterizerMode::DIRECTX)
	{
		RGBColor clearColor = RGBColor{ 0.f,0.f,0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}
	else
	{
		SDL_LockSurface(m_pBackBuffer);
		SDL_FillRect(m_pBackBuffer, NULL, 0x00004D);

		std::fill(m_DepthBuffer.begin(), m_DepthBuffer.end(), FLT_MAX);
	}

	//Draw
	for (Mesh* geo : meshes)
	{
		if(!geo->GetIsEnabled()) continue;
		worldMat = geo->GetWorldMatrix();
		Material* pMat = geo->GetMaterial();
		

		if (gameS.GetRasterizerMode() == RasterizerMode::DIRECTX)
		{
			//Get effect variable and needed data to pass to the GPU

			ID3D11Buffer* pVertexBuffer = geo->GetVertexBuffer();
			ID3D11Buffer* pIndexBuffer = geo->GetIndexBuffer();
			ID3D11InputLayout* pInputLayout = geo->GetInputLayout();

			//Load Shader variables
			pMat->SetEffectVariables(projMat, viewMat, worldMat);

			//Set vertex buffer
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

			//set index buffer
			m_pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			//Set the input layout
			m_pDeviceContext->IASetInputLayout(pInputLayout);

			//Set primitive topology
			m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Render a triangle
			D3DX11_TECHNIQUE_DESC techDesc;
			pMat->GetCurrentTechnique()->GetDesc(&techDesc);
			for (UINT p = 0; p < techDesc.Passes; ++p)
			{
				pMat->GetCurrentTechnique()->GetPassByIndex(p)->Apply(0, m_pDeviceContext);
				m_pDeviceContext->DrawIndexed(geo->GetVerticesAmount(), 0, 0);
			}
		}
		else
		{
			const std::vector<Vertex>& vertices = geo->GetVertices();
			const std::vector<uint32_t>& indices = geo->GetIndices();
			Vertex v0{}, v1{}, v2{};

			auto iter = indices.begin();
			auto end = indices.end() - 3;
			HitInfo hit{};
			VS_OUTPUT output{};
			VS_WORLDPOS worldPos{};
			output.mesh = geo;
			float depthVal{};

			while (iter < end)
			{
				v0 = vertices[*iter++];
				v1 = vertices[*iter++];
				v2 = vertices[*iter++];

				VertexTransformationFunc(v0, v1, v2, worldPos, projMat, viewMat, worldMat);

				FMatrix2 bBox = GetBoundingBox(v0, v1, v2);

				if(!IsInFrustum(bBox)) continue;
				if(!IsInViewRange(v0, v1, v2)) continue;

				TransformToRasterSpace(v0, v1, v2, bBox);

				if(!IsInScreenBorders(bBox)) continue;

				uint32_t xmin = std::max(int32_t(0), int32_t(bBox[0][0]) - 1);
				uint32_t xmax = std::min(int32_t(m_Width), int32_t(bBox[0][1]) + 1);
				uint32_t ymin = std::max(int32_t(0), int32_t(bBox[1][1]) - 1);
				uint32_t ymax = std::min(int32_t(m_Height), int32_t(bBox[1][0]) + 1);

				for (uint32_t y = ymin; y < ymax; ++y)
				{
					for (uint32_t x = xmin; x < xmax; ++x)
					{
						hit.screenPos = { float(x), float(y) };

						if (PointInTriangle(hit, v0, v1, v2, output, worldPos))
						{
							//if(hit.depthValue < 0.001f || hit.depthValue > 1.f) continue;
							depthVal = m_DepthBuffer[x + y * size_t(m_Width)];

							if (hit.depthValue < depthVal)
							{
								m_DepthBuffer[x + y * size_t(m_Width)] = hit.depthValue;
								hit.color = PixelShader(output);

								hit.color.MaxToOne();

								//Fill the pixels - pixel access demo
								m_pBackBufferPixels[x + (y * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
									static_cast<uint8_t>(hit.color.r * 255.f),
									static_cast<uint8_t>(hit.color.g * 255.f),
									static_cast<uint8_t>(hit.color.b * 255.f));
							}
							hit.color = {};
							hit.depthValue = FLT_MAX;
							hit.screenPos = {};
						}
					}
				}
			}
		}
	}


	//Present
	if (gameS.GetRasterizerMode() == RasterizerMode::DIRECTX)
		m_pSwapChain->Present(0, 0);
	else
	{
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
	}
}

bool Elite::Renderer::PointInTriangle(HitInfo& hit, const Vertex& v0, const Vertex& v1, const Vertex& v2, VS_OUTPUT& out, const VS_WORLDPOS& worldPos)
{
	FVector2 vToP = hit.screenPos - v1.pos.xy;
	FVector2 vToV = v2.pos.xy - v1.pos.xy;

	float sArea0 = Cross(vToV, vToP);

	if (sArea0 < -0.00001f) return false;

	vToP = hit.screenPos - v2.pos.xy;
	vToV = v0.pos.xy - v2.pos.xy;

	float sArea1 = Cross(vToV, vToP);

	if (sArea1 < -0.00001f) return false;

	vToP = hit.screenPos - v0.pos.xy;
	vToV = v1.pos.xy - v0.pos.xy;

	float sArea2 = Cross(vToV, vToP);

	if (sArea2 < -0.00001f) return false;

	vToP = v0.pos.xy - v2.pos.xy;
	vToV = v0.pos.xy - v1.pos.xy;
	float totalArea = Cross(vToV, vToP);

	sArea0 /= totalArea;
	sArea1 /= totalArea;
	sArea2 /= totalArea;

	hit.depthValue = ((1 / v0.pos.z) * sArea0 + (1 / v1.pos.z) * sArea1 + (1 / v2.pos.z) * sArea2);

	float Wlerp = 1 /((1 / v0.pos.w) * sArea0 + (1 / v1.pos.w) * sArea1 + (1 / v2.pos.w) * sArea2);
	out.uv = { ((v0.uv / v0.pos.w) * sArea0 + (v1.uv / v1.pos.w) * sArea1 + (v2.uv / v2.pos.w) * sArea2) * Wlerp };
	out.normal = GetNormalized(v0.normal * sArea0 + v1.normal * sArea1 + v2.normal * sArea2);
	out.tangent = GetNormalized(v0.tangent * sArea0 + v1.tangent * sArea1 + v2.tangent * sArea2);
	out.worldPos = FPoint4(worldPos.v0 * sArea0 + worldPos.v1 * sArea1 + worldPos.v2 * sArea2 );
	return true;
}

Elite::RGBColor Elite::Renderer::PixelShader(VS_OUTPUT& input)
{
	GameSettings& gameS = GameSettings::GetInstance();
	Material* pMat = input.mesh->GetMaterial();
	Texture* pDiffuseMap = pMat->GetDiffuseTexture();
	Texture* pNormalMap = pMat->GetNormalTexture();
	Texture* pSpecularMap = pMat->GetSpecularTexture();
	Texture* pGlossMap = pMat->GetGlossTexture();
	FMatrix4 viewInverse = Inverse(gameS.GetCamera()->GetViewMatrix());

	FVector3 viewDir = GetNormalized(FVector3{ input.worldPos.xyz - viewInverse[3].xyz });
	FVector3 newNormal = input.normal;
	if(pNormalMap != nullptr)
		newNormal = CalculateNewNormal(input.tangent, input.normal, input.uv, pNormalMap);

	RGBColor specularColor{};
	if(pSpecularMap != nullptr && pGlossMap != nullptr)
		specularColor = CalculateSpecularPhong(viewDir, newNormal, input.uv, pSpecularMap, pGlossMap);

	RGBColor diffuseColor{};
	if(pDiffuseMap != nullptr)
		diffuseColor = CalculateDiffuse(newNormal, input.uv, pDiffuseMap);

	return diffuseColor + specularColor;
}

Elite::FVector3 Elite::Renderer::CalculateNewNormal(const FVector3& tangent, const FVector3& normal, const FVector2& uv, Texture* pNormalMap)
{
	FVector3 newNormal = normal;

	FVector3 binormal = GetNormalized(Cross(tangent, normal));
	FMatrix3 localAxis{ tangent, binormal, normal };

	RGBColor sample = pNormalMap->SampleRGB(uv);
	FVector3 sampledNormal = FVector3{ sample.r, sample.g, sample.b };
	sampledNormal = sampledNormal * 2.f;
	sampledNormal.x -= 1.f;
	sampledNormal.y -= 1.f;
	sampledNormal.z -= 1.f;

	newNormal = GetNormalized(localAxis * sampledNormal);

	return newNormal;
}

Elite::RGBColor Elite::Renderer::CalculateSpecularPhong(const FVector3& viewDir, const FVector3& normal, const FVector2& uv, Texture* pSpecularMap, Texture* pGlossMap)
{
	DirectionalLight dirLight = GameSettings::GetInstance().GetDirectionalLight();
	FVector3 reflectedLightDir = Reflect(dirLight.GetLightDirection(), normal);
	float specularStrength = Dot(-viewDir, reflectedLightDir);
	float shininess = pGlossMap->SampleRGB(uv).r * 25.f;
	specularStrength = Clamp(specularStrength,0.f,1.f);
	specularStrength = powf(specularStrength, shininess);

	RGBColor specularColor = pSpecularMap->SampleRGB(uv) * specularStrength;

	return specularColor;
}

Elite::RGBColor Elite::Renderer::CalculateDiffuse(const FVector3& normal, const FVector2& uv, Texture* pDiffuseMap)
{
	DirectionalLight DirLight = GameSettings::GetInstance().GetDirectionalLight();

	float diffuseLightValue = std::max(Dot(-normal, DirLight.GetLightDirection()) * DirLight.GetLightIntensity(), 0.f);
	RGBColor diffuseColor = pDiffuseMap->SampleRGB(uv) * diffuseLightValue;
	return diffuseColor;
}

void Elite::Renderer::VertexTransformationFunc(Vertex& v0, Vertex& v1, Vertex& v2, VS_WORLDPOS& worldPos, const FMatrix4& projMat, const FMatrix4& viewMat, const FMatrix4& worldMat)
{
	FMatrix4 VPWMat = projMat * viewMat * worldMat;
	FMatrix3 rotWorld{ worldMat };

	v0.pos.z *= -1.f;
	v1.pos.z *= -1.f;
	v2.pos.z *= -1.f;

	worldPos.v0 = FVector4(worldMat * v0.pos);
	worldPos.v1 = FVector4(worldMat * v1.pos);
	worldPos.v2 = FVector4(worldMat * v2.pos);

	v0.pos = VPWMat * v0.pos;
	v1.pos = VPWMat * v1.pos;
	v2.pos = VPWMat * v2.pos;

	v0.normal.z *= -1.f;
	v1.normal.z *= -1.f;
	v2.normal.z *= -1.f;

	v0.normal = rotWorld * v0.normal;
	v1.normal = rotWorld * v1.normal;
	v2.normal = rotWorld * v2.normal;

	v0.tangent.z *= -1.f;
	v1.tangent.z *= -1.f;
	v2.tangent.z *= -1.f;

	v0.tangent = rotWorld * v0.tangent;
	v1.tangent = rotWorld * v1.tangent;
	v2.tangent = rotWorld * v2.tangent;

	v0.pos.xyz /= v0.pos.w;
	v1.pos.xyz /= v1.pos.w;
	v2.pos.xyz /= v2.pos.w;
}

Elite::FMatrix2 Elite::Renderer::GetBoundingBox(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	FMatrix2 boundingBox{};

	//first column then row
	boundingBox[0][0] = std::min(v0.pos.x, std::min(v1.pos.x, v2.pos.x));
	boundingBox[0][1] = std::max(v0.pos.x, std::max(v1.pos.x, v2.pos.x));
	boundingBox[1][0] = std::min(v0.pos.y, std::min(v1.pos.y, v2.pos.y));
	boundingBox[1][1] = std::max(v0.pos.y, std::max(v1.pos.y, v2.pos.y));

	return boundingBox;
}

bool Elite::Renderer::IsInFrustum(const FMatrix2& bBox)
{
	if (bBox[0][1] < -1.f || bBox[0][0] > 1.f || bBox[1][1] > 1.f || bBox[1][0] < -1.f)
		return false;
	else
		return true;
}

bool Elite::Renderer::IsInScreenBorders(const FMatrix2& bBox)
{
	if (bBox[0][1] < 0.f || bBox[1][1] < 0.f || bBox[0][0] > float(m_Width - 1) || bBox[1][0] > float(m_Height - 1))
		return false;
	else
		return true;
}

bool Elite::Renderer::IsInViewRange(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	float depth = (v0.pos.z + v1.pos.z + v2.pos.z) / 3.f;
	if (depth > 1.f && depth < 0.1f) 
		return false;
	else
		return true;
}

void Elite::Renderer::TransformToRasterSpace(Vertex& v0, Vertex& v1, Vertex& v2, FMatrix2& bBox)
{
	//put the position in raster space
	v0.pos.x = (v0.pos.x + 1.f) * 0.5f * float(m_Width);
	v1.pos.x = (v1.pos.x + 1.f) * 0.5f * float(m_Width);
	v2.pos.x = (v2.pos.x + 1.f) * 0.5f * float(m_Width);
	v0.pos.y = (1.f - v0.pos.y) * 0.5f * float(m_Height);
	v1.pos.y = (1.f - v1.pos.y) * 0.5f * float(m_Height);
	v2.pos.y = (1.f - v2.pos.y) * 0.5f * float(m_Height);

	bBox[0] = FVector2{ bBox[0][0] + 1.f, bBox[0][1] + 1.f } * 0.5f * float(m_Width);
	bBox[1] = FVector2{ 1.f - bBox[1][0], 1.f - bBox[1][1] } * 0.5f * float(m_Height);
}

void Elite::Renderer::InitializeSDL()
{
	m_pFrontBuffer = SDL_GetWindowSurface(m_pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_DepthBuffer.resize(size_t(m_Width) * m_Height, FLT_MAX);
}

HRESULT Elite::Renderer::InitializeDirectX()
{
#pragma warning(push)
#pragma warning(disable : 26812)
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
#pragma warning(pop)

	uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);

	if (FAILED(result)) return result;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result)) return result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	SDL_SysWMinfo sysWMinfo{};
	SDL_VERSION(&sysWMinfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
	swapChainDesc.OutputWindow = sysWMinfo.info.win.window;

	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result)) return result;

	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result)) return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result)) return result;

	//Create the RenderTargetView
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result)) return result;

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result)) return result;

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return result;
}

