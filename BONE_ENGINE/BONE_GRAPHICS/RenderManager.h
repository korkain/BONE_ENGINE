#pragma once
#include "Common.h"
#include "ISingleton.h"
#include <MultiThreadSync.h>

namespace BONE_GRAPHICS
{
    enum SHADER_ENUM { SHADER_SHAODW_CREATE, SHADER_SHADOW_APPLY };

    typedef struct _RAY
    {
        D3DXVECTOR3	origin;
        D3DXVECTOR3	direction;
        _RAY();
    } RAY;

    class RenderManager : public ISingleton<RenderManager>, public BONE_SYSTEM::MultiThreadSync<RenderManager>
    {
    private:
        HWND					hWnd;
        IDirect3D9*				d3d9;
        D3DCAPS9				caps;
        D3DPRESENT_PARAMETERS	d3dpp;
        IDirect3DDevice9*		D3D_DEVICE;
        int						width, height;
        ID3DXLine*				line;
        ID3DXFont*				font;
        bool                    useImGUI;
        std::string             lastRenderedMesh;

        bool MSAAModeSupported(D3DMULTISAMPLE_TYPE type, D3DFORMAT backBufferFmt,
            D3DFORMAT depthStencilFmt, BOOL windowed,
            DWORD &qualityLevels);

        void ChooseBestMSAAMode(D3DFORMAT backBufferFmt, D3DFORMAT depthStencilFmt,
            BOOL windowed, D3DMULTISAMPLE_TYPE &type,
            DWORD &qualityLevels, DWORD &samplesPerPixel);

    public:
        RenderManager() {}
        virtual ~RenderManager() {}

        bool InitializeMembers(HWND hWnd, bool useImGUI);
        virtual void ReleaseMembers();

        IDirect3DDevice9* GetDevice();

        std::string GetLastRenderedMesh();
        void SetLastRenderedMesh(std::string name);

        int GetHeight();
        int GetWidth();
        int GetMatrixPaletteSize();
        D3DXVECTOR3 GetScreenPos(const D3DXVECTOR3& pos, D3DXMATRIX* view, D3DXMATRIX* proj);

        void RenderText(std::string text, D3DXVECTOR2 pos, int length, const char* font, int opt, int setting, D3DXCOLOR color);

        RAY	GetPickingRayToView(bool isMouseCenter);
        RAY	GetPickingRay(bool isMouseCenter);
        RAY	TransRayToView(RAY ray);

        bool CheckRayInMesh(const D3DXMATRIX& matWorld, LPD3DXMESH mesh, float* dist = nullptr);
        bool CheckRayInTriangle(RAY* ray, const D3DXMATRIX& matWorld, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, float* u, float* v, float* dist = nullptr);

        void SetupPixelFog(bool on, D3DXCOLOR color, float start = 0.5f, float end = 0.8f, float density = 0.66f, int mode = D3DFOG_LINEAR);

        void DrawLine(D3DXMATRIX matrix, D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXCOLOR color);
        void DrawLine(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXCOLOR color);

        bool DrawMeshBox(D3DXMATRIX matrix, LPD3DXMESH mesh, D3DXVECTOR3 pos, D3DXCOLOR color);

        void DrawBox(D3DXMATRIX transform, D3DXVECTOR3 leftBottom, D3DXVECTOR3 rightTop, D3DXCOLOR color);
        void DrawSphere(D3DXMATRIX transform, float radius, D3DXCOLOR color);
        void DrawCylinder(D3DXMATRIX transform, float radius, float height, D3DXCOLOR color);
        void DrawCone(D3DXMATRIX transform, float edge, float edge2, float edge3, D3DXCOLOR color);

        bool UseImGUI();
        void UseImGUI(bool use);
    };
}