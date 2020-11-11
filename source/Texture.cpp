#include "pch.h"
#include "Texture.h"
#include <SDL_image.h>

Elite::Texture::Texture()
	: m_pSurface(nullptr)
	, m_pTexture(nullptr)
	, m_pTextureResourceView(nullptr)
{
}

Elite::Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);
	m_pTexture->Release();
	m_pTextureResourceView->Release();
}

Elite::RGBColor Elite::Texture::SampleRGB(const FVector2& uv)
{
	uint32_t x{ uint32_t(uv.x * m_pSurface->w) }, y{ uint32_t( uv.y * m_pSurface->h) };
	uint32_t pixeloffset{ x + y * m_pSurface->w };
	if(pixeloffset > uint32_t(m_pSurface->w * m_pSurface->h)) pixeloffset = uint32_t(m_pSurface->w * m_pSurface->h);
	Uint32 pixel{*((Uint32*)m_pSurface->pixels + pixeloffset)};

	Uint8 r{}, g{}, b{};

	SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);

	return RGBColor{ r / 255.f, g / 255.f, b / 255.f };
}

Elite::FPoint4 Elite::Texture::SampleRGBA(const FVector2& uv)
{
	uint32_t x{ uint32_t(uv.x * m_pSurface->w) }, y{ uint32_t(uv.y * m_pSurface->h) };
	uint32_t pixeloffset{ x + y * m_pSurface->w };
	if (pixeloffset > uint32_t(m_pSurface->w * m_pSurface->h)) pixeloffset = uint32_t(m_pSurface->w * m_pSurface->h);
	Uint32 pixel{ *((Uint32*)m_pSurface->pixels + pixeloffset) };

	Uint8 r{}, g{}, b{}, a{};

	SDL_GetRGBA(pixel, m_pSurface->format, &r, &g, &b,&a);

	return FPoint4{ r / 255.f, g / 255.f, b / 255.f , a / 255.f};
}

bool Elite::Texture::LoadTexture(const std::string& filePath, ID3D11Device* pDevice)
{
	m_pSurface = IMG_Load(filePath.c_str());
	if (m_pSurface == nullptr) return false;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);

	if (FAILED(hr)) return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceView);

	if (FAILED(hr))return false;

	return true;
}
