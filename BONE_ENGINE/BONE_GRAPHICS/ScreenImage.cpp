#include "Common.h"
#include "ResourceManager.h"
#include "Transform2D.h"
#include "ScreenImage.h"

namespace BONE_GRAPHICS
{
	ScreenImage::ScreenImage()
	{
		ThreadSync sync;

		SetTypeName("ScreenImage");

		alpha = 255;

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}
	
	ScreenImage::~ScreenImage()
	{
		ThreadSync sync;

		sprite->Release();
	}

	void ScreenImage::LoadContent()
	{
		ThreadSync sync;

		ResourceMgr->LoadTexture(address);
	}

	void ScreenImage::SetOriginRect(Rect _rect)
	{
		ThreadSync sync;

		originRect = _rect;
	}

	void ScreenImage::SetOriginRect(Vector2 _leftUp, Vector2 _rightBottom)
	{
		ThreadSync sync;

		originRect.LeftTop = _leftUp;
		originRect.RightBottom = _rightBottom;
	}

	Rect ScreenImage::GetOriginRect()
	{
		ThreadSync sync;

		return originRect;
	}

	void ScreenImage::SetImageFile(string _address)
	{
		ThreadSync sync;

		address = _address;
	}

	void ScreenImage::SetAlpha(float _alpha)
	{
		ThreadSync sync;

		alpha = _alpha;
	}
	
	void ScreenImage::Render(GameObject* _owner)
	{
		ThreadSync sync;

		Matrix matrix = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetTransform();
		Vector3 position = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetPosition();

		RECT rect;
		rect.left = originRect.LeftTop.x;
		rect.top = originRect.LeftTop.y;
		rect.right = originRect.RightBottom.x;
		rect.bottom = originRect.RightBottom.y;

		LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(address);

		sprite->SetTransform(&matrix);

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		// alpha - normal 
		RenderMgr->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);   //3D��

		D3DCOLOR RGB = D3DCOLOR_ARGB((int)alpha, 255, 255, 255);
		sprite->Draw(texture, &rect, nullptr, &Vector3(0, 0, 0), RGB);
		sprite->End();

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	
	float ScreenImage::GetAlpha()
	{
		ThreadSync sync;

		return alpha;
	}
}