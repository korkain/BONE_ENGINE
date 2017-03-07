#pragma once
#include "Common.h"
#include "Skybox.h"
#include "Camera.h"
#include "etuImage.h"
#include "GameObject.h"
#include "Skybox.h"

namespace BONE_GRAPHICS
{
	class Scene
	{
	private:
		std::list<GameObject*> objectList;
		std::list<GameObject*> staticObjectList;
		etuImage image_LoadingBackground;
		bool IsFrameworkFlag;
		int cameraIndex;
		Skybox skybox;
		bool CompleateLoading;
 
	public:
		bool InitializeMembers();
		void Reference();
		bool LoadContents();
		void Render(double timeDelta);
		void Update(double timeDelta);
		void LateUpdate(double timeDelta);
		void LateRender();

	public:
		Scene();
		~Scene();
		
		void AddObject(GameObject* object);
		void AddObjects(GameObject** objects, int size);
		
		void SortPriorityObject();

		std::tuple<GameObject**, int> FindObjectsByTag(string tag);
		GameObject* FindObjectByTag(string tag);
		void Destroy(GameObject* gameObject);

		GameObject* GetCurrentCamera();

		void SetCamera(int ID);
		bool SetLoading(string imageAddress, int width, int height);

		void SetSkybox(string dirName, string fileType);

		bool GetSceneFlag();
		void SetSceneFlag(bool flag);
		bool EndLoading();

		void SetCameraIndex(int index);
		int GetCameraIndex();

	};
}