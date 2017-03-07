#include "Common.h"
#include "SceneManager.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	void SceneManager::InitializeMembers()
	{
		ThreadSync sync;

		closeThread = false;
		loadingBarValue = 0;
		loadScene = "";
		frame = 0;
	}

	void SceneManager::LoadingRender() 
	{
		ThreadSync sync;
	}
	
	void LoadThreadFunc(Scene* Framework)
	{
		Framework->LoadContents();
	}

	void SceneManager::AddScene(string name, Scene* scene)
	{
		ThreadSync sync;

		sceneList.insert(std::pair<string, Scene*>(name, scene));
	}

	Scene* SceneManager::CurrentScene()
	{
        return sceneList[curScene];
	}

	Scene* SceneManager::GetLoadScene()
	{
		return sceneList[loadScene];
	}

	void SceneManager::SetLoadingScene(string name)
	{
		ThreadSync sync;

		loadScene = name;
	}

	int SceneManager::GetFrame()
	{
		return frame;
	}

	double SceneManager::GetTimeDelta()
	{
		return timeDelta;
	}
	
	bool SceneManager::StartScene(string name)
	{
		curScene = name;
		sceneList[name]->InitializeMembers();

		std::thread LoadingThread(LoadThreadFunc, sceneList[name]);

		double lastTime = (double)timeGetTime();

		if (sceneList[loadScene] != NULL)
		{
			sceneList[loadScene]->InitializeMembers();
			sceneList[loadScene]->Reference();
			sceneList[loadScene]->LoadContents();

			do
			{
				double currTime = (double)timeGetTime();
				double deltaTime = (currTime - lastTime) * 0.001f;
				timeDelta = deltaTime;

				RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
				RenderMgr->GetDevice()->BeginScene();

				if (sceneList[loadScene]->GetSceneFlag())
				{
					sceneList[loadScene]->Update(deltaTime);
					sceneList[loadScene]->LateUpdate(deltaTime);
					sceneList[loadScene]->Render(deltaTime);
					sceneList[loadScene]->LateRender();
				}

				RenderMgr->GetDevice()->EndScene();
				RenderMgr->GetDevice()->Present(0, 0, 0, 0);

				InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_NONE);

				lastTime = currTime;

			} while (!sceneList[name]->EndLoading() || sceneList[loadScene]->GetSceneFlag());

		}

		LoadingThread.join();

		sceneList[name]->Reference();

		sceneList[name]->SortPriorityObject();

		lastTime = (double)timeGetTime();

		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		double OneSecond = 0;
		int Frame = 0;

		while (msg.message != WM_QUIT)
		{
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				if (sceneList[name]->GetSceneFlag())
				{
					double currTime = (double)timeGetTime();
					double deltaTime = (currTime - lastTime) * 0.001f;
					timeDelta = deltaTime;

					RenderMgr->GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, backColor, 1.0f, 0);
					RenderMgr->GetDevice()->BeginScene();

					if (sceneList[name]->GetSceneFlag())
					{
						sceneList[name]->Update(deltaTime);
						sceneList[name]->LateUpdate(deltaTime);
						sceneList[name]->Render(deltaTime);
						sceneList[name]->LateRender();
					}

					RenderMgr->GetDevice()->EndScene();
					RenderMgr->GetDevice()->Present(0, 0, 0, 0);

					InputMgr->SetMouseWheelStatus(MOUSE_WHEEL_NONE);

					lastTime = currTime;

					OneSecond += deltaTime;
					Frame++;

					if (OneSecond >= 1.0f)
					{
						frame = Frame;
						Frame = 0;
						OneSecond = 0;
					}
				}
				else
					break;
			}
		}

		closeThread = true;

		if (sceneList[name]->GetSceneFlag() == false)
		{
			sceneList.erase(loadScene);
			loadScene = "";
			sceneList.erase(name);
			return true;
		}

		sceneList.erase(loadScene);
		loadScene = "";
		sceneList.erase(name);

		return false;
	}

	void SceneManager::EndScene(string name)
	{
		ThreadSync sync;

		sceneList[name]->SetSceneFlag(false);
	}

	void SceneManager::ReleaseMembers()
	{
	}
}