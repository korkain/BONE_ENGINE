#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class IShader : public Component, public MultiThreadSync<IShader>
	{
	protected:
		LPD3DXEFFECT* shader;
		string address;

	public:
		IShader();
		virtual ~IShader();

		void LoadContent();
		void SetAddress(string _address);
		string GetAddress();

		virtual void Render(int i, GameObject* parent) = 0;

		LPD3DXEFFECT GetShader();
	};
}