#include "Common.h"
#include "PointLight.h"
#include "Scene.h"
#include "Transform3D.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    PointLight::PointLight()
    {
        radius = 10;
        this->status = false;
        IsInit = false;
    }

    PointLight::~PointLight()
    {

    }

    void PointLight::Init()
    {
        SetTag("PointLight");
        SetPriority(100);

        Transform3D* tr = new Transform3D();
        tr->SetPosition(pos);
        this->AddComponent(tr);

        this->tr = tr;

        IsInit = true;
    }

    void PointLight::Reference()
    {
        if (this->status)
            SceneMgr->CurrentScene()->AddPointLight(this);
        else
            SceneMgr->CurrentScene()->RemovePointLight(this);
    }

    void PointLight::SetLight(bool status)
    {
        if (this->status == status)
            return;

        this->status = status;

        if (!IsInit)
            return;

        if (this->status)
            SceneMgr->CurrentScene()->AddPointLight(this);
        else
            SceneMgr->CurrentScene()->RemovePointLight(this);
    }

    void PointLight::SetAmbient(float r, float g, float b, float a)
    {
        ambient.r = r;
        ambient.g = g;
        ambient.b = b;
        ambient.a = a;
    }

    void PointLight::SetDiffuse(float r, float g, float b, float a)
    {
        diffuse.r = r;
        diffuse.g = g;
        diffuse.b = b;
        diffuse.a = a;
    }
    
    void PointLight::SetSpecular(float r, float g, float b, float a)
    {
        specular.r = r;
        specular.g = g;
        specular.b = b;
        specular.a = a;
    }
    
    void PointLight::SetRadius(float value)
    {
        radius = value;
    }

    void PointLight::SetAmbient(RGBA color)
    {
        ambient = color;
    }

    void PointLight::SetDiffuse(RGBA color)
    {
        diffuse = color;
    }
    
    void PointLight::SetSpecular(RGBA color)
    {
        specular = color;
    }

    void PointLight::SetPosition(Vector3 pos)
    {
        this->pos = pos;
    }

    RGBA PointLight::GetAmbient()
    {
        return ambient;
    }

    RGBA PointLight::GetDiffuse()
    {
        return diffuse;
    }
    
    RGBA PointLight::GetSpecular()
    {
        return specular;
    }

    Vector3 PointLight::GetPosition()
    {
        return tr->GetPosition();
    }
    
    float PointLight::GetRadius()
    {
        return radius;
    }
}