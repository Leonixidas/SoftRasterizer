#pragma once
#include <string>
#include <vector>

namespace Elite
{
	class Mesh;
	class Camera;

	class Scene
	{
	public:
		explicit Scene(const std::string& sceneName);
		virtual ~Scene();

		void RootInitialize(ID3D11Device* pDevice);
		void RootUpdate();

		virtual void Initialize(ID3D11Device* pDevice) = 0;
		virtual void Update() = 0;
		virtual void Render(ID3D11DeviceContext* pDeviceContext);

		const std::vector<Mesh*>& GetMeshes() { return m_Meshes; }

		void AddMesh(Mesh* mesh);

		void AddCamera(Camera* cam);

	private:
		std::string m_SceneName;
		bool m_IsInitialized;
		std::vector<Mesh*> m_Meshes;
		std::vector<Camera*> m_Cameras;
		size_t m_CurrentCamIndex;
	};
}
