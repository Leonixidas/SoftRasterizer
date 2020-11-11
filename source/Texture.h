#pragma once
#include <string>

struct SDL_Surface;

namespace Elite
{

	class Texture
	{
	public:
		Texture();
		~Texture();

		RGBColor SampleRGB(const FVector2& uv);
		FPoint4 SampleRGBA(const FVector2& uv);

		bool LoadTexture(const std::string& filePath, ID3D11Device* pDevice);

		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pTextureResourceView; }

	private:
		SDL_Surface* m_pSurface;
		ID3D11Texture2D* m_pTexture;
		ID3D11ShaderResourceView* m_pTextureResourceView;
	};
}

