#include "Common.h"
#include "ResourceManager.h"
#include "Transform2D.h"
#include "ScreenImage.h"

namespace BONE_GRAPHICS
{
	ScreenImage::ScreenImage()
	{
		SetTypeName("ScreenImage");

        myalpha = 255;

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}
	
	ScreenImage::~ScreenImage()
	{
		sprite->Release();
	}

	void ScreenImage::LoadContent()
	{
		ResourceMgr->LoadTexture(fileName);
	}

	void ScreenImage::SetOriginRect(Rect rect)
	{
		originRect = rect;
	}

	void ScreenImage::SetOriginRect(Vec2 leftUp, Vec2 rightBottom)
	{
		originRect.LeftTop = leftUp;
		originRect.RightBottom = rightBottom;
	}

	Rect ScreenImage::GetOriginRect()
	{
		return originRect;
	}

	void ScreenImage::SetImageFile(std::string fileName)
	{
		this->fileName = fileName;
	}

	void ScreenImage::SetAlpha(float alpha)
	{
		myalpha = alpha;
	}
	
	void ScreenImage::Render(GameObject* owner)
	{
        if (owner->GetActive())
        {
            Matrix matrix = GET_TRANSFORM_2D(owner)->GetTransform();
            Vec3 position = GET_TRANSFORM_2D(owner)->GetPosition();

            RECT rect;
            rect.left = originRect.LeftTop.x;
            rect.top = originRect.LeftTop.y;
            rect.right = originRect.RightBottom.x;
            rect.bottom = originRect.RightBottom.y;

            LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(fileName);

            sprite->SetTransform(&matrix);

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            RenderMgr->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

            sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

            D3DCOLOR RGB = D3DCOLOR_ARGB((int)(myalpha), 255, 255, 255);
            sprite->Draw(texture, &rect, nullptr, &Vec3(0, 0, 0), RGB);
            sprite->End();

            RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }
	}
	
	float ScreenImage::GetAlpha()
	{
		return myalpha;
	}
}