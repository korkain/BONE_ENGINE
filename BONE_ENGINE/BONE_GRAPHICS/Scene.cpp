#include "Common.h"
#include "Scene.h"
#include "LogManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "Transform3D.h"
#include "PhysicsListener.h"
#include "etuImage.h"
#include "Rp3dRigidBody.h"
#include "StaticMesh.h"
#include "SoundManager.h"

#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
    Scene::Scene()
    {
        IsFrameworkFlag = false;
        cameraIndex = 0;
        CompleateLoading = false;
        onLoadScene = false;

        globalAmbient.r = 1.0f;
        globalAmbient.g = 1.0f;
        globalAmbient.b = 1.0f;
        globalAmbient.a = 1.0f;

        accumulator = 0;

        enableFog = false;
        fogStart = 50.0f;
        fogEnd = 100.0f;
        fogDensity = 1.0f;
        fogMode = D3DFOG_LINEAR;

        objectList.clear();
        staticObjectList.clear();

        fogColor = COLOR::YELLOW;

        rp3d::Vector3 gravity(0.0, -9.81, 0.0);
        rp3d::DynamicsWorld test(gravity);
        physicsWorld = new rp3d::DynamicsWorld(gravity);

        enablePhysics = false;
        isEditorScene = false;

        physicsWorld->setNbIterationsVelocitySolver(15);
        physicsWorld->setNbIterationsPositionSolver(8);
        physicsWorld->enableSleeping(true);
        physicsWorld->setSleepAngularVelocity(1.0f);
        physicsWorld->setSleepLinearVelocity(1.0f);
        physicsWorld->setTimeBeforeSleep(0.1f);
        physicsWorld->setEventListener(&physicsEventListner);
    }

    Scene::~Scene()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end();)
        {
            GameObject* Temp = *Iter;

            if (Temp != nullptr)
            {
                Iter = objectList.erase(Iter);
                delete Temp;
            }
            else
                Iter++;
        }

        for (auto Iter = pointLightList.begin(); Iter != pointLightList.end();)
        {
            GameObject* Temp = *Iter;

            if (Temp != nullptr)
            {
                Iter = pointLightList.erase(Iter);
                delete Temp;
            }
            else
                Iter++;
        }

        delete physicsWorld;
        physicsWorld = nullptr;

        RelShader();
    }

    void Scene::SetEditorScene()
    {
        isEditorScene = true;
    }
    
    bool Scene::IsEditorScene()
    {
        return isEditorScene;
    }

    void Scene::EnablePhysics(bool enable)
    {
        enablePhysics = enable;
    }

    bool Scene::IsEnablePhysics()
    {
        return enablePhysics;
    }

    rp3d::DynamicsWorld* Scene::GetPhysicsWorld()
    {
        return physicsWorld;
    }

    bool Scene::SetLoading(std::string imageAddress, int width, int height)
    {
        RECT rect = { 0, 0, width, height };
        return image_LoadingBackground.SetInformaition("image_LoadingBackground", imageAddress, D3DXVECTOR3(0, 0, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), &rect, NULL);
    }

    bool Scene::InitializeMembers()
    {
        InitShader();

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            (*Iter)->Init();

        IsFrameworkFlag = true;
                
        return IsFrameworkFlag;
    }

    bool Scene::LoadContents()
    {
        float Max = objectList.size();
        float Cur = 0;
        loadPerTime = 0;
        
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
        {
            Cur++;
            (*Iter)->LoadContents();

            loadPerTime = Cur / Max;
        }
        
        if (Max == 0)
            loadPerTime = 1;
        
        IsFrameworkFlag = true;
        CompleateLoading = true;
        
        return IsFrameworkFlag;
    }

    float Scene::GetLoadPerTime()
    {
        return loadPerTime * 100;
    }

    void Scene::LateRender()
    {
        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateRender();

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateRender();
    }

    void Scene::SetSkybox(std::string dirName, std::string fileType)
    {
        char Path[MAX_PATH];
        strcpy_s(Path, dirName.c_str());
        skybox.SetSkybox(Path, fileType);
    }

    GameObject* Scene::GetCurrentCamera()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (((Camera*)(*Iter)->GetComponent("Camera")) != NULL)
                if (((Camera*)(*Iter)->GetComponent("Camera"))->GetID() == SceneMgr->CurrentScene()->GetCameraIndex())
                    return (*Iter);
    }

    void Scene::Reference()
    {
        for (auto Iter = graphNodeList.begin(); Iter != graphNodeList.end(); Iter++)
            (*Iter).second->Reference();

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            (*Iter)->Reference();

        for (auto Iter = pointLightList.begin(); Iter != pointLightList.end(); Iter++)
            (*Iter)->Reference();
    }

    void Scene::Update()
    {
        //// Get all the contacts of the world 
        //auto manifolds = physicsWorld->getContactsList();

        //// For each contact manifold of the body 
        //for (auto it = manifolds.begin(); it != manifolds.end(); ++it) {
        //    auto manifold = *it;

        //    // For each contact point of the manifold 
        //    for (int i = 0; i<manifold->getNbContactPoints(); i++) {

        //        // Get the contact point 
        //        ContactPoint* point = manifold->getContactPoint(i);
        //        cout << manifold->getShape1()->getCollisionShape()->name << ", " << manifold->getShape2()->getCollisionShape()->name << endl;
        //        
        //        // Get the world-space contact point on body 1 
        //        Vector3 pos = point->getWorldPointOnBody1();

        //        // Get the world-space contact normal 
        //        Vector3 normal = point->getNormal();
        //    }
        //}

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->Update();
    }

    void Scene::PhysicsUpdate(double timeDelta)
    {
        if (enablePhysics)
        {
            for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
                if (GET_RIGIDBODY((*Iter)) != nullptr)
                    GET_RIGIDBODY((*Iter))->LockUpdate();

            const float timeStep = 1.0 / 60.0;
            accumulator += timeDelta;

            while (accumulator >= timeStep) {
                physicsWorld->update(timeStep);
                accumulator -= timeStep;
            }
        }
    }

    void Scene::LateUpdate()
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if ((*Iter)->GetActive())
                (*Iter)->LateUpdate();
    }

    void Scene::AddObject(GameObject* object, std::string name)
    {
        SceneMgr->SetGameObject(object);

        object->SetName(name);

        if (object->GetStatic())
            staticObjectList.push_back(object);
        else
            objectList.push_back(object);

        if (CompleateLoading)
        {
            object->Init();
            object->Reference();
            object->LoadContents();
        }
    }

    class ObjSortClass {
    public:
        bool operator() (GameObject* left, GameObject* right) const {
            return left->GetPriority() < right->GetPriority();
        }
    };

    class ObjSortClass2 {
    public:
        bool operator() (GameObject* left, GameObject* right) const {
            auto leftCom = GET_STATIC_MESH(left);
            auto rightCom = GET_STATIC_MESH(right);

            if (leftCom != nullptr && rightCom != nullptr)
                return leftCom->GetFile() < rightCom->GetFile();
            
            if (leftCom == nullptr && rightCom != nullptr)
                return true;

            if (leftCom != nullptr && rightCom == nullptr)
                return false;

            return false;
        }
    };

    void Scene::SortPriorityObject()
    {
        objectList.sort(ObjSortClass());
        staticObjectList.sort(ObjSortClass());
    }

    void Scene::SortRenderOptimizeObject()
    {
        objectList.sort(ObjSortClass2());
        staticObjectList.sort(ObjSortClass2());
     }

    void Scene::AddObjects(GameObject** objects, int size)
    {
        for (int i = 0; i < size; i++)
        {
            if (objects[i]->GetStatic())
                staticObjectList.push_back(objects[i]);
            else
                objectList.push_back(objects[i]);

            if (CompleateLoading)
            {
                objects[i]->Init();
                objects[i]->Reference();
                objects[i]->LoadContents();
            }
        }
    }

    bool Scene::EndLoading()
    {
        return CompleateLoading;
    }

    GameObject* Scene::FindObjectByTag(std::string tag)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                return (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                return (*Iter);

        return nullptr;
    }

    GameObject* Scene::FindObjectByName(std::string name)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (name == (*Iter)->GetName())
                return (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (name == (*Iter)->GetName())
                return (*Iter);

        return nullptr;
    }

    std::tuple<GameObject**, int> Scene::FindObjectsByTag(std::string tag)
    {
        std::tuple<GameObject**, int> Result;

        GameObject** ObjArray = new GameObject*[objectList.size() + staticObjectList.size()];
        int Size = 0;

        for (int i = 0; i < objectList.size() + staticObjectList.size(); i++)
            ObjArray[i] = NULL;

        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                ObjArray[Size++] = (*Iter);

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
            if (tag == (*Iter)->Tag())
                ObjArray[Size++] = (*Iter);

        std::get<0>(Result) = ObjArray;
        std::get<1>(Result) = Size;

        return Result;
    }

    bool Scene::GetSceneFlag()
    {
        return IsFrameworkFlag;
    }

    void Scene::Destroy(GameObject* gameObject)
    {
        for (auto Iter = objectList.begin(); Iter != objectList.end(); Iter++)
        {
            if (gameObject == (*Iter))
            {
                objectList.remove(gameObject);
                break;
            }
        }

        for (auto Iter = staticObjectList.begin(); Iter != staticObjectList.end(); Iter++)
        {
            if (gameObject == (*Iter))
            {
                staticObjectList.remove(gameObject);
                break;
            }
        }

        delete gameObject;
    }

    void Scene::SetSceneFlag(bool flag)
    {
        IsFrameworkFlag = flag;
    }

    void Scene::SetCameraIndex(int index)
    {
        cameraIndex = index;
    }

    int Scene::GetCameraIndex()
    {
        return cameraIndex;
    }

    void Scene::SetName(std::string name)
    {
        this->name = name;
    }

    std::string Scene::GetName()
    {
        return name;
    }

    void Scene::SaveSceneData()
    {
        bool isNewMap = false;
        std::string FullPath;
                
        json j;

        j["Scene"]["GlobalAmbient"] = {
            globalAmbient.r,
            globalAmbient.g,
            globalAmbient.b,
            globalAmbient.a
        };

        j["Scene"]["Skybox"]["Folder"] = skybox.GetFolderName();
        j["Scene"]["Skybox"]["Type"] = skybox.GetFileType();

        j["Scene"]["EnableFog"] = enableFog;

        if (enableFog)
        {
            j["Scene"]["FogStart"] = fogStart;
            j["Scene"]["FogEnd"] = fogEnd;
            j["Scene"]["FogDensity"] = fogDensity;
            j["Scene"]["FogMode"] = fogMode;
            j["Scene"]["FogColor"] = {
                fogColor.r,
                fogColor.g,
                fogColor.b,
                fogColor.a
            };
        }

        j["Scene"]["BackGroundColor"] = {
            SceneMgr->GetClearColor().r,
            SceneMgr->GetClearColor().g,
            SceneMgr->GetClearColor().b,
            SceneMgr->GetClearColor().a
        };
        
        std::string fullPath = ".\\Engine\\Maps\\" + SceneMgr->CurrentScene()->GetName() + ".json";
        std::ofstream o(fullPath);
        o << std::setw(4) << j << std::endl;
        o.close();

        for each(auto var in pointLightList)
        {
            ((PointLight*)var)->SaveInMaps();
        }

        for each(auto var in graphNodeList)
        {
            ((GraphNode*)var.second)->SaveInMaps();
        }
    }

    void Scene::ClearSceneData()
    {
        std::string FullPath;

        if (!ResourceMgr->ExistFile(name + ".json", &FullPath))
            return;

        _unlink(FullPath.c_str());
    }

    void Scene::LoadSceneData()
    {
        if (!onLoadScene)
            return;

        std::string FullPath;

        if (!ResourceMgr->ExistFile(name + ".json", &FullPath))
            return;

        std::ifstream file(FullPath);
        json j;
        file >> j;

        auto GlobalAmbient = j["Scene"]["GlobalAmbient"].get<std::vector<double>>();
        
        globalAmbient.r = GlobalAmbient[0];
        globalAmbient.g = GlobalAmbient[1];
        globalAmbient.b = GlobalAmbient[2];
        globalAmbient.a = GlobalAmbient[3];
        
        if (j["Scene"]["Skybox"]["Folder"] != "None")
        {
            char SkyboxFolder[MAX_PATH] = "";
            strcpy(SkyboxFolder, j["Scene"]["Skybox"]["Folder"].get<std::string>().c_str());

            skybox.SetSkybox(SkyboxFolder, j["Scene"]["Skybox"]["Type"].get<std::string>());
        }

        auto EnableFog = j["Scene"]["EnableFog"].get<bool>();
        enableFog = EnableFog;

        if (enableFog)
        {
            fogStart = j["Scene"]["FogStart"].get<double>();
            fogEnd = j["Scene"]["FogEnd"].get<double>();
            fogDensity = j["Scene"]["FogDensity"].get<double>();
            fogMode = j["Scene"]["FogMode"].get<double>();
            
            auto FogColor = j["Scene"]["FogColor"].get<std::vector<double>>();

            fogColor.r = FogColor[0];
            fogColor.r = FogColor[1];
            fogColor.r = FogColor[2];
            fogColor.r = FogColor[3];
        }

        auto BackGroundColor = j["Scene"]["BackGroundColor"].get<std::vector<double>>();
        D3DXCOLOR color;
        color.r = BackGroundColor[0];
        color.g = BackGroundColor[1];
        color.b = BackGroundColor[2];
        color.a = BackGroundColor[3];
        SceneMgr->SetClearColor(color);

        int i = 0;
        for (json::iterator it = j["Light"].begin(); it != j["Light"].end(); ++it) {
            auto Position = j["Light"][it.key()]["Position"].get<std::vector<double>>();
            auto Ambient = j["Light"][it.key()]["Ambient"].get<std::vector<double>>();
            auto Diffuse = j["Light"][it.key()]["Diffuse"].get<std::vector<double>>();
            auto Specular = j["Light"][it.key()]["Specular"].get<std::vector<double>>();
            auto Radius = j["Light"][it.key()]["Radius"].get<double>();
            auto TargetPos = j["Light"][it.key()]["ShadowTarget"].get<std::vector<double>>();
            auto Status = j["Light"][it.key()]["Status"].get<bool>();

            pointLightList[i]->SetPosition(Vec3(Position[0], Position[1], Position[2]));
            pointLightList[i]->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
            pointLightList[i]->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
            pointLightList[i]->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
            pointLightList[i]->SetRadius(Radius);
            pointLightList[i]->SetShadowAimPos(Vec3(TargetPos[0], TargetPos[1], TargetPos[2]));
            pointLightList[i]->SetLight(Status);
            i++;
        }

        for (json::iterator it = j["GraphNode"].begin(); it != j["GraphNode"].end(); ++it) {
            GraphNode* node = new GraphNode();
            node->Init();
            node->LoadContents();
            
            auto Position = j["GraphNode"][it.key()]["Position"].get<std::vector<double>>();
            node->SetPosition(Vec3(Position[0], Position[1], Position[2]));

            if (j["GraphNode"][it.key()].find("Connections") != j["GraphNode"][it.key()].end())
            {
                auto ConnectionNodes = j["GraphNode"][it.key()]["Connections"].get<std::vector<std::string>>();

                for each (auto var in ConnectionNodes)
                    node->ConnectNode(var);
            }

            node->SetName(it.key());
            graphNodeList.insert(std::pair<std::string, GraphNode*>(it.key(), node));
        }

        for (json::iterator it = j["GameObject"].begin(); it != j["GameObject"].end(); ++it) {
            auto Object = this->FindObjectByName(it.key());

            if (Object == nullptr)
            {
                if (it->find("IsChild").value().get<bool>())
                    continue;

                Object = new GameObject();
                Transform3D* tr = new Transform3D();
                Object->AddComponent(tr);

                Object->SetName(it.key());

                Object->SetPriority(1);
                Object->SetTag("");

                Object->SetPrfabName(it->find("PrefabName").value().get<std::string>());
                Object->LoadPrefab();
                Object->Init();
                
                this->AddObject(Object, it.key());

                auto Position = it->find("Position").value().get<std::vector<float>>();
                tr->SetPosition(Position[0], Position[1], Position[2]);

                auto Rotation = it->find("Rotation").value().get<std::vector<float>>();
                tr->SetRotate(Rotation[0], Rotation[1], Rotation[2]);

                auto Scale = it->find("Scale").value().get<std::vector<float>>();
                    tr->SetScale(Scale[0], Scale[1], Scale[2]);
            }
            else
            {
                Object->SetPrfabName(it->find("PrefabName").value().get<std::string>());
                Object->LoadPrefab();
                Object->Init();

                auto Position = it->find("Position").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetPosition(Position[0], Position[1], Position[2]);

                auto Rotation = it->find("Rotation").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetRotate(Rotation[0], Rotation[1], Rotation[2]);

                auto Scale = it->find("Scale").value().get<std::vector<float>>();
                ((Transform3D*)Object->GetComponent("Transform3D"))->SetScale(Scale[0], Scale[1], Scale[2]);
            }
        }

        file.close();
    }

    std::map<std::string, GraphNode*>* Scene::GetGraphNodes()
    {
        return &graphNodeList;
    }
    
    void Scene::AddGraphNode(GraphNode* node)
    {
        graphNodeList.insert(std::pair<std::string, GraphNode*>(node->GetName(), node));
    }

    void Scene::RemoveGraphNode(GraphNode* node)
    {
        for (auto iter = graphNodeList.begin(); iter != graphNodeList.end(); iter++)
        {
            if ((*iter).second == node)
            {
                graphNodeList.erase(iter);
                break;
            }
        }
    }

    Vec3 Scene::FindNodeByDistance(Vec3 pos)
    {
        return Vec3(0, 0, 0);
    }

    void Scene::SetAmbientColor(float r, float g, float b, float a)
    {
        globalAmbient.r = r;
        globalAmbient.g = g;
        globalAmbient.b = b;
        globalAmbient.a = a;
    }

    void Scene::SetAmbientColor(RGBA color)
    {
        globalAmbient = color;
    }

    RGBA Scene::GetAmbientColor()
    {
        return globalAmbient;
    }

    void Scene::OnLoadSceneData()
    {
        onLoadScene = true;
    }

    std::vector<PointLight*> Scene::GetPointLights()
    {
        return pointLightList;
    }

    std::list<GameObject*> Scene::GetObjectList()
    {
        return objectList;
    }

    std::list<GameObject*> Scene::GetStaticObjectList()
    {
        return staticObjectList;
    }

    void Scene::SetFogStatus(bool on, D3DXCOLOR color, float fogStart, float fogEnd, float fogDensity, int mode)
    {
        this->enableFog = on;
        this->fogStart = fogStart;
        this->fogEnd = fogEnd;
        this->fogMode = mode;
        this->fogDensity = fogDensity;
        this->fogColor = color;

        RenderMgr->SetupPixelFog(on, color, fogStart, fogEnd, fogDensity, mode);
    }

    void Scene::CreateShadowMap()
    {
        createShadowMap = false;
    }
    
    bool Scene::OnFog()
    {
        return enableFog;
    }

    float Scene::GetFogDensity()
    {
        return fogDensity;
    }
    
    float Scene::GetFogStart()
    {
        return fogStart;
    }

    float Scene::GetFogEnd()
    {
        return fogEnd;
    }
    
    int Scene::GetFogMode()
    {
        return fogMode;
    }

    D3DXCOLOR Scene::GetFogColor()
    {
        return fogColor;
    }
}