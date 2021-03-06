#include "Common.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "SpriteBillBoard.h"
#include "Camera.h"
#include "GameObject.h"
#include "Vertex.h"

namespace BONE_GRAPHICS
{
	SpriteBillBoard::SpriteBillBoard()
	{
		SetTypeName("SpriteBillBoard");

		vertexBuffer = nullptr;
		indexBuffer = nullptr;

		rectWidth = 1;
		rectHeight = 1;
        speed = 1;

		renderMode = RENDER_ALPHA;

		rightPlay = true;
        isTargetCamera = true;
        CutTimer = 0;
	}

	SpriteBillBoard::~SpriteBillBoard()
	{
		if(vertexBuffer == nullptr)
			vertexBuffer->Release();
		
		if(indexBuffer == nullptr)
			indexBuffer->Release();

		ResourceMgr->ReleaseTexture(fileName);
	}

	void SpriteBillBoard::LoadContent()
	{
		VERTEX vertex[4];

#pragma region Vertex Setup

		vertex[0].p.x = rectWidth/2;
		vertex[0].p.y = rectHeight/2;
		vertex[0].p.z = 0;

		vertex[0].n = vertex[0].p;
		D3DXVec3Normalize(&vertex[0].n, &vertex[0].n);

		vertex[0].uv.x = (float)curRect.left / (float)width;
		vertex[0].uv.y = (float)curRect.top / (float)height;

		vertex[1].p.x = -rectWidth / 2;
		vertex[1].p.y = rectHeight / 2;
		vertex[1].p.z = 0;

		vertex[1].n = vertex[1].p;
		D3DXVec3Normalize(&vertex[1].n, &vertex[1].n);

		vertex[1].uv.x = (float)curRect.right / (float)width;
		vertex[1].uv.y = (float)curRect.top / (float)height;

		vertex[2].p.x = -rectWidth / 2;
		vertex[2].p.y = -rectHeight / 2;
		vertex[2].p.z = 0;

		vertex[2].n = vertex[2].p;
		D3DXVec3Normalize(&vertex[2].n, &vertex[2].n);

		vertex[2].uv.x = (float)curRect.right / (float)width;
		vertex[2].uv.y = (float)curRect.bottom / (float)height;

		vertex[3].p.x = rectWidth / 2;
		vertex[3].p.y = -rectHeight / 2;
		vertex[3].p.z = 0;

		vertex[3].n = vertex[3].p;
		D3DXVec3Normalize(&vertex[3].n, &vertex[3].n);

		vertex[3].uv.x = (float)curRect.left / (float)width;
		vertex[3].uv.y = (float)curRect.bottom / (float)height;

#pragma endregion

#pragma region Vertex/Index Buffer Setup
		if (vertexBuffer != nullptr)
		{
			vertexBuffer->Release();
			vertexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
		{
			return;
		}

		VOID* pVertices;

		if (FAILED(vertexBuffer->Lock(0, 4 * sizeof(VERTEX), (void**)&pVertices, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
			return;

		VERTEX* pV = (VERTEX*)pVertices;

		for (int i = 0; i < 4; i++)
			*(pV++) = vertex[i];

		vertexBuffer->Unlock();

		if (indexBuffer != nullptr)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer( 2 * sizeof(VERTEX_INDEX),
			0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr)))
			return;

		VERTEX_INDEX IB;
		int IB_Index = 0;
		VOID* pVertices2;

		if (FAILED(indexBuffer->Lock(0, 2 * sizeof(VERTEX_INDEX), (void**)&pVertices2, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
			return;

		VERTEX_INDEX* pI = (VERTEX_INDEX*)pVertices2;

		IB._0 = IB_Index;
		IB._1 = IB_Index + 1;
		IB._2 = IB_Index + 3;
		*(pI++) = IB;

		IB._0 = IB_Index + 3;
		IB._1 = IB_Index + 1;
		IB._2 = IB_Index + 2;
		*(pI++) = IB;

		indexBuffer->Unlock();

#pragma endregion

		IsInit = true;
	}

	void SpriteBillBoard::SetPlayDirection(bool isRight)
	{
		rightPlay = isRight;
	}

	void SpriteBillBoard::SetRectSize(float width, float height)
	{
		rectWidth = width;
		rectHeight = height;
	}

	Vec2 SpriteBillBoard::GetRectSize()
	{
		return Vec2(rectWidth, rectHeight);
	}

    void SpriteBillBoard::IsFullAnimation(bool isFull)
    {
        fullAnimations = isFull;
    }
    
    bool SpriteBillBoard::IsFullAnimation()
    {
        return fullAnimations;
    }

    SPRITE_INFO SpriteBillBoard::GetSpriteInfo()
    {
        SPRITE_INFO info;

        info.width = width;
        info.height = height;
        info.animationCut = animeCut;
        info.animationScene = animeScene;

        return info;
    }

	void SpriteBillBoard::SetAnimation(int width, int height, int animationCut, int animationScene, float alpha)
	{
		this->width = width;
        this->height = height;
		animeCut = animationCut;
		animeScene = animationScene;

		RECT rect = {
			0, 0, width / animeCut, height / animeScene,
		};

		RECT Margin = { 0, 0, 0, 0 };

		curAnimeCut = 0;
		curAnimeScene = 0;
		this->alpha = alpha;

		RECT rect2 = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect2;
	}

	void SpriteBillBoard::PlayCutAnimation(float cutTimer)
	{
		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;

		if (CutTimer > cutTimer)
		{
			if (rightPlay)
			{
				if (curAnimeCut >= animeCut - 1)
					curAnimeCut = 0;
				else
					curAnimeCut++;
			}
			else
			{
				if (curAnimeCut <= 0)
					curAnimeCut = animeCut - 1;
				else
					curAnimeCut--;
			}

			CutTimer = 0;
		}
		else
			CutTimer += SceneMgr->GetTimeDelta() * speed;

		LoadContent();
	}

	void SpriteBillBoard::PlayFullAnimation(float cutTimer)
	{
		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;
        
		if (CutTimer > cutTimer)
		{
			if (curAnimeCut >= animeCut - 1)
			{
				curAnimeCut = 0;

				if (curAnimeScene >= animeScene - 1)
					curAnimeScene = 0;
				else
					curAnimeScene++;

			}
			else
				curAnimeCut++;

			CutTimer = 0;
		}
		else
			CutTimer += SceneMgr->GetTimeDelta() * speed;

		LoadContent();
	}
	
	void SpriteBillBoard::SelectAnimation(int sceneIndex)
	{
		if (curAnimeScene != sceneIndex)
		{
			curAnimeScene = sceneIndex;
			curAnimeCut = 0;
		}
	}
	
	bool SpriteBillBoard::CheckMouseRayInMesh(Transform3D* tr)
	{
		RAY ray = RenderMgr->GetPickingRayToView(false);

		return true;
	}

	void SpriteBillBoard::SetTexture(std::string fileName)
	{
		this->fileName = fileName;
	}

	std::string	SpriteBillBoard::GetTexture()
	{
		return fileName;
	}

	void SpriteBillBoard::SetTargetCamera(bool isTargetCamera)
	{
        this->isTargetCamera = isTargetCamera;
	}

	void SpriteBillBoard::SetRenderMode(RENDER_MODE mode)
	{
		renderMode = mode;
	}

    void SpriteBillBoard::SetSpeed(float speed)
    {
        this->speed = speed;
    }

    float SpriteBillBoard::GetSpeed()
    {
        return speed;
    }
    
    float SpriteBillBoard::GetAlpha()
    {
        return alpha;
    }

    bool SpriteBillBoard::IsTargetCamera()
    {
        return isTargetCamera;
    }

	void SpriteBillBoard::Render(IShader* shaderOpt, GameObject* object)
	{
        if (IsInit)
        {
            if (isTargetCamera) {
                Vec3 Dir =
                    ((Camera*)(CUR_SCENE->GetCurrentCamera()->GetComponent("Camera")))->GetTargetPosition()
                    - ((Transform3D*)(CUR_SCENE->GetCurrentCamera()->transform3D))->GetPosition();
                D3DXVec3Normalize(&Dir, &Dir);

                float x = Dir.x;
                float y = Dir.y;
                float z = Dir.z;

                float YAngle = asin(z / (sqrt(pow(x, 2) + pow(z, 2))));
                float ZAngle = asin(y / (sqrt(pow(x, 2) + pow(y, 2))));

                ZAngle = 0;

                if (Dir.x >= 0 && Dir.z >= 0)
                {
                    YAngle = asin(z / (sqrt(pow(x, 2) + pow(z, 2))));
                }
                else if (Dir.x < 0 && Dir.z >= 0)
                {
                    YAngle = asin(-x / (sqrt(pow(x, 2) + pow(z, 2))));
                    YAngle += 3.14f / 2;
                }
                else if (Dir.z < 0 && Dir.x < 0)
                {
                    YAngle = asin(-z / (sqrt(pow(x, 2) + pow(z, 2))));
                    YAngle += 3.14f;
                }
                else if (Dir.z < 0 && Dir.x >= 0)
                {
                    YAngle = asin(x / (sqrt(pow(x, 2) + pow(z, 2))));
                    YAngle += 3.14f * (3.0f / 2.0f);
                }

                YAngle += 3.14f + (3.0f / 2.0f);

                ((Transform3D*)object->GetComponent("Transform3D"))->SetRotate(0, -YAngle, -ZAngle);
            }

			Matrix Transform = ((Transform3D*)object->GetComponent("Transform3D"))->GetTransform();
			RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &Transform);

			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);

				RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);

				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
			}
			else if (renderMode == RENDER_STENCIL)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x00);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			}

			RenderMgr->GetDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(VERTEX));
			RenderMgr->GetDevice()->SetIndices(indexBuffer);

			if (shaderOpt == nullptr)
			    RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(fileName));
			else
				shaderOpt->Render(0, object);
			
			RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
			RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
			}
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);

		}
	}
}