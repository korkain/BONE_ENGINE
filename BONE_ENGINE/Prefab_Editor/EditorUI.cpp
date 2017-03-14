#include "EditorUI.h"
#include <Common.h>
#include <SceneManager.h>
#include <Transform3D.h>
#include <Camera.h>
#include <Material.h>
#include <TrailRenderer.h>
#include <BillBoard.h>
#include <SpriteBillBoard.h>
#include <StaticMesh.h>
#include <SkinnedMesh.h>
#include <Collision.h>
#include <GameObject.h>
#include <InputManager.h>
#include <Collision.h>

using namespace BONE_GRAPHICS;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

EditorUI::EditorUI() {
    open = true;
    currentShowInfoObject = "";
    currentObjectName = "";
    showAddComponent = false;
    showObjectInfo = false;
    childSize = 0;
    showPrefabHierarchical = false;
}

void EditorUI::ShowFileMenu()
{
    if (ImGui::MenuItem("New")) 
    {
    }
    
    if (ImGui::MenuItem("Open", "Ctrl+O")) 
    {
    }
    
    if (ImGui::MenuItem("Save", "Ctrl+S")) 
    {
        SceneMgr->CurrentScene()->ClearSceneData();

        auto DyamicObjectList = SceneMgr->CurrentScene()->GetObjectList();
        auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();
        
        for each(auto var in DyamicObjectList)
        {
            if (var->GetName() != "EditorCamera")
            {
                var->SavePrefab();
                var->SaveInMaps();
            }
        }

        for each(auto var in StaticObjectList)
        {
            if (var->GetName() != "EditorCamera")
            {
                var->SavePrefab();
                var->SaveInMaps();
            }
        }
    }
    
    if (ImGui::MenuItem("Save As..")) 
    {
    }
    
    if (ImGui::MenuItem("Quit", "Alt+F4")) 
    {
        SceneMgr->CurrentScene()->SetSceneFlag(true);
        SceneMgr->EndScene(SceneMgr->CurrentScene()->GetName());
    }
}

void EditorUI::ShowOpitionMenu()
{
    if (ImGui::BeginMenu("RenderMode"))
    {
        if (ImGui::MenuItem("POLYGON")) 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        
        if (ImGui::MenuItem("WIRE_FRAME")) 
            RenderMgr->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Collision")) {}
}

void EditorUI::ShowHelpMenu()
{
    if (ImGui::BeginMenu("RenderMode"))
    {
        if (ImGui::MenuItem("POLYGON")) {}
        if (ImGui::MenuItem("WIRE_FRAME")) {}

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Show Collision")) {}
}

void EditorUI::ShowGameObjectTree(std::string treeName)
{
    if (ImGui::TreeNode(treeName.c_str()))
    {
        auto parent = SceneMgr->CurrentScene()->FindObjectByName(treeName);
        
        if (ImGui::SmallButton("Show Infos"))
        {
            currentShowInfoObject = treeName;
            showObjectInfo = true;
        }

        if (treeName != currentObjectName)
        {
            if (ImGui::SmallButton("Remove Object"))
            {
                parent->DetachParent();
                SceneMgr->CurrentScene()->Destroy(parent);
            }
        }

        if (ImGui::TreeNode("Add New Child"))
        {
            static char Name[64] = "";
            ImGui::InputText("Name", Name, 64);

            if (ImGui::SmallButton("Create"))
            {
                GameObject* Child = new GameObject();

                Transform3D* tr = new Transform3D();
                Child->AddComponent(tr);

                if (!strcmp("Name", ""))
                {
                    std::string ChildName = "Child";

                    char temp[10] = "";
                    itoa(childSize, temp, 10);
                    ChildName += temp;
                
                    Child->SetPrfabName(ChildName);
                    Child->SetName(ChildName);
                    Child->SetPriority(1);
                    SceneMgr->CurrentScene()->AddObject(Child, ChildName);
                }
                else
                {
                    Child->SetPrfabName(Name);
                    Child->SetName(Name);
                    Child->SetPriority(1);
                    SceneMgr->CurrentScene()->AddObject(Child, Name);
                }

                parent->AttachChild(Child);
                childSize++;
            }

            ImGui::TreePop();
        }

        auto childs = parent->GetChileds();

        for (auto iter = childs.begin(); iter != childs.end(); iter++)
            ShowGameObjectTree((*iter)->GetName());

        ImGui::TreePop();
    }
}

void EditorUI::AllChildCheck(GameObject* parent)
{
    auto childs = parent->GetChileds();

    for (auto iter = childs.begin(); iter != childs.end(); iter++)
    {
        if (ImGui::TreeNode((*iter)->GetName().c_str()))
        {
            AllChildCheck((*iter));
            ImGui::TreePop();
        }
    }
}

void EditorUI::UpdateFrame()
{
    {
        if (!ImGui::Begin("Editor", &open, ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        if (ImGui::IsRootWindowOrAnyChildFocused())
            InputMgr->SetFocusWindow(false);
        else
            InputMgr->SetFocusWindow(true);

        std::list<GameObject*> ObjectList = SceneMgr->CurrentScene()->GetObjectList();
        auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();
        ObjectList.sort();
        StaticObjectList.sort();
        ObjectList.merge(StaticObjectList);

        if (ImGui::CollapsingHeader("[Scene Entities]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool FindOption;
            ImGui::Checkbox("Enable Find Option", &FindOption);
            
            std::list<std::string> FindObjectName;

            if (FindOption)
            {
                std::list<std::string> objectNames;

                for (auto iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
                {
                    if ((*iter)->GetParent() != nullptr)
                        continue;

                    objectNames.push_back((*iter)->GetName());
                }

                static ImGuiTextFilter filter;
                filter.Draw();

                for (auto iter = objectNames.begin(); iter != objectNames.end(); iter++)
                    if (filter.PassFilter(iter->c_str()))
                        FindObjectName.push_back(iter->c_str());
            }

            for (auto iter = ObjectList.begin(); iter != ObjectList.end(); iter++)
            {
                if ((*iter)->GetParent() != nullptr || (*iter)->GetName() == "EditorCamera")
                    continue;

                if (FindOption)
                {
                    bool Exist = false;

                    for (auto iter2 = FindObjectName.begin(); iter2 != FindObjectName.end(); iter2++)
                        if ((*iter2) == (*iter)->GetName())
                            Exist = true;

                    if (!Exist)
                        continue;
                }

                if (ImGui::TreeNode((*iter)->GetName().c_str()))
                {
                    if (ImGui::TreeNode("Information"))
                    {
                        auto IsActive = (*iter)->GetActive();
                        ImGui::Checkbox("IsActvie", &IsActive);
                        ImGui::SameLine();

                        auto IsStatic = (*iter)->GetStatic();
                        ImGui::Checkbox("IsStatic", &IsStatic);

                        auto Priority = (*iter)->GetPriority();
                        ImGui::InputInt("Priority", &Priority);

                        auto Tag = (*iter)->Tag();
                        char TagStr[64] = "";
                        strcpy(TagStr, Tag.c_str());
                        ImGui::InputText("Tag", TagStr, 64);

                        (*iter)->SetActive(IsActive);
                        (*iter)->SetStatic(IsStatic);
                        (*iter)->SetPriority(Priority);
                        (*iter)->SetTag(Tag);

                        static char buf[64] = "";
                        ImGui::InputText("Name", buf, 64);
                        ImGui::SameLine();

                        if (ImGui::Button("Change"))
                        {
                            auto object = SceneMgr->CurrentScene()->FindObjectByName((*iter)->GetName().c_str());
                            object->SetName(buf);
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Prefab"))
                    {
                        auto Prefabs = ResourceMgr->ExistFiles(".\\Engine\\Prefabs\\*");
                            
                        const int Size = Prefabs.size();
                        char** ComboBoxItems = new char*[Size];
                        
                        int i = 0;
                        for each(auto item in Prefabs)
                        {
                            ComboBoxItems[i] = new char[64];
                            strcpy(ComboBoxItems[i], Prefabs[i].c_str());
                            i++;
                        }

                        static int CurItem = 0;
                        
                        for (int i = 0; i < Size; i++)
                        {
                            if ((*iter)->GetPrfabName() + ".json" == Prefabs[i])
                            {
                                CurItem = i;
                                break;
                            }
                        }

                        ImGui::Combo("Prefabs", &CurItem, ComboBoxItems, Size);
                        
                        static char PrefabName[64] = "";
                        ImGui::InputText("PrefabName", PrefabName, 64);
                        
                        if (ImGui::SmallButton("New File"))
                        {
                            (*iter)->SetPrfabName(PrefabName);
                            (*iter)->SavePrefab();
                        }
                        
                        if (ImGui::SmallButton("Edit Prfab"))
                        {
                            for (int i = Prefabs[CurItem].size(); i >= 0; i--)
                            {
                                if (ComboBoxItems[CurItem][i] != '.')
                                    ComboBoxItems[CurItem][i] = '\0';
                                else
                                {
                                    ComboBoxItems[CurItem][i] = '\0';
                                    break;
                                }
                            }
                            (*iter)->SetPrfabName(ComboBoxItems[CurItem]);

                            showPrefabHierarchical = true;

                            currentObjectName = (*iter)->GetName();
                        }
                        
                        for (int i = 0; i < Size; i++)
                            delete ComboBoxItems[i];
                        delete[] ComboBoxItems;
                    
                        ImGui::TreePop();
                    }
                    
                    if ((*iter)->GetChileds().size() != 0)
                    {
                        if (ImGui::TreeNode("Childs"))
                        {
                            AllChildCheck((*iter));
                            ImGui::TreePop();
                        }
                    }

                    if (ImGui::SmallButton("Remove Object"))
                    {
                        SceneMgr->CurrentScene()->Destroy((*iter));
                    }

                    if (ImGui::SmallButton("Focus On"))
                    {
                    }

                    ImGui::TreePop();
                }
            }

            if (ImGui::TreeNode("New GameObject"))
            {
                auto Prefabs = ResourceMgr->ExistFiles(".\\Engine\\Prefabs\\*");

                const int Size = Prefabs.size();
                char** ComboBoxItems = new char*[Size];

                int i = 0;
                for each(auto item in Prefabs)
                {
                    ComboBoxItems[i] = new char[64];
                    strcpy(ComboBoxItems[i], Prefabs[i].c_str());
                    i++;
                }

                static int CurItem = 0;
                ImGui::Combo("Prefabs", &CurItem, ComboBoxItems, Size);

                static char PrefabName[64] = "";
                static bool NewPrefabs = false;
                ImGui::InputText("PrefabName", PrefabName, 64);
                ImGui::Checkbox("New Prefab", &NewPrefabs);

                if (ImGui::Button("Create"))
                {
                    GameObject* Object = new GameObject;

                    std::string ObjectName = "GameObject";
                    static int ObjectNum = 0;

                    char temp[10] = "";
                    itoa(ObjectNum, temp, 10);
                    ObjectName += temp;
                    ObjectNum++;

                    if (NewPrefabs)
                    {
                        Object->SetName(ObjectName);
                        Object->SetPriority(1);
                        Object->SetTag("");

                        Transform3D* tr = new Transform3D();
                        Object->AddComponent(tr);
                        Object->SetPrfabName(PrefabName);
                        Object->SavePrefab();
                        SceneMgr->CurrentScene()->AddObject(Object, ObjectName);
                    }
                    else if (Size != 0)
                    {
                        for (int i = Prefabs[CurItem].size(); i >= 0; i--)
                        {
                            if (ComboBoxItems[CurItem][i] != '.')
                                ComboBoxItems[CurItem][i] = '\0';
                            else
                            {
                                ComboBoxItems[CurItem][i] = '\0';
                                break;
                            }
                        }
                        Object->SetPrfabName(ComboBoxItems[CurItem]);
                        Object->LoadPrefab();

                        SceneMgr->CurrentScene()->AddObject(Object, ObjectName);
                    }
                }

                for (int i = 0; i < Size; i++)
                    delete ComboBoxItems[i];
                delete[] ComboBoxItems;

                ImGui::TreePop();
            }
        }
        
        if (ImGui::CollapsingHeader("[Environment Setting]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::TreeNode("SkyBox"))
            {
                static char SkyBoxFolder[64] = "miramar";
                ImGui::InputText("Folder Name", SkyBoxFolder, 64);

                static char TypeName[64] = "tga";
                ImGui::InputText("ImageType", TypeName, 64);

                if (ImGui::Button("Chanage SkyBox"))
                    SceneMgr->CurrentScene()->SetSkybox(SkyBoxFolder, TypeName);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("World Ambient"))
            {
                auto OriAmbient = SceneMgr->CurrentScene()->GetAmbientColor();

                static float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                
                ImGui::InputFloat3("Ambient", Ambient);

                SceneMgr->CurrentScene()->SetAmbientColor(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Terrain"))
            {
                ImGui::TreePop();
            }
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ShowFileMenu();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Option"))
            {
                ShowOpitionMenu();
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Help")) {}

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    if (showPrefabHierarchical)
    {
        std::string WindowName = "Prefab Inspector : " + currentObjectName;
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showPrefabHierarchical);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            InputMgr->SetFocusWindow(false);
        else
            InputMgr->SetFocusWindow(true);

        ShowGameObjectTree(currentObjectName);

        ImGui::End();
    }

    if (showObjectInfo)
    {
        std::string WindowName = "GameObject Info : " + currentShowInfoObject;
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showObjectInfo);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            InputMgr->SetFocusWindow(false);
        else
            InputMgr->SetFocusWindow(true);

        if (ImGui::CollapsingHeader("[GameObject Info]", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static char buf[64] = "";
            ImGui::InputText("Name", buf, 64);
            ImGui::SameLine();

            if (ImGui::Button("Change"))
            {
                auto object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                currentShowInfoObject = buf;

                object->SetName(currentShowInfoObject);
            }
        }

        if (ImGui::CollapsingHeader("[Components]"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            if (ImGui::SmallButton("Add Component"))
                showAddComponent = true;
            
            GameObject* object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
            std::vector<Component*> components = object->GetComponents();

            for (auto iter = components.begin(); iter != components.end(); iter++)
            {
                if (ImGui::TreeNode((*iter)->GetTypeName().c_str()))
                {
                    if ((*iter)->GetTypeName() == "Transform3D")
                    {
                        Vector3 oriPos = ((Transform3D*)object->transform3D)->GetPosition();
                        Vector3 oriRot = ((Transform3D*)object->transform3D)->GetRotateAngle();
                        Vector3 oriScale = ((Transform3D*)object->transform3D)->GetScale();
                        Vector3 oriForward = ((Transform3D*)object->transform3D)->GetForward();

                        float pos[3] = { oriPos.x, oriPos.y, oriPos.z };
                        float rot[3] = { oriRot.x, oriRot.y, oriRot.z };
                        float scale[3] = { oriScale.x, oriScale.y, oriScale.z };
                        float forward[3] = { oriForward.x, oriForward.y, oriForward.z };
                        
                        ImGui::InputFloat3("Position", pos);
                        ImGui::InputFloat3("Rotation", rot);
                        ImGui::InputFloat3("Scale", scale);
                        ImGui::InputFloat3("Forward", forward);
                        
                        ((Transform3D*)object->transform3D)->SetPosition(pos[0], pos[1], pos[2]);
                        ((Transform3D*)object->transform3D)->SetRotate(rot[0], rot[1], rot[2]);
                        ((Transform3D*)object->transform3D)->SetScale(scale[0], scale[1], scale[2]);
                        ((Transform3D*)object->transform3D)->SetForward(forward[0], forward[1], forward[2]);
                    }
                    else if ((*iter)->GetTypeName() == "StaticMesh")
                    {
                        auto MeshName = ((StaticMesh*)(*iter))->GetFileAddress();
                        auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\Mesh\\*");
                        
                        const int Size = Meshes.size();
                        char** ComboBoxItems = new char*[Size];
                        
                        int CurItem = 0;

                        int i = 0;
                        for each(auto item in Meshes)
                        {
                            ComboBoxItems[i] = new char[64];
                            strcpy(ComboBoxItems[i], Meshes[i].c_str());
                            
                            if (MeshName == ComboBoxItems[i]) 
                                CurItem = i;

                            i++;
                        }

                        ImGui::Combo("Meshes", &CurItem, ComboBoxItems, Size);
                        
                        if (ImGui::Button("Chanage"))
                        {
                            
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Remove"))
                        {

                        }

                        for (int i = 0; i < Size; i++)
                            delete ComboBoxItems[i];
                        delete[] ComboBoxItems;
                    }
                    else if ((*iter)->GetTypeName() == "Material")
                    {
                        RGBA OriAmbient = ((Material*)(*iter))->GetAmbient();
                        RGBA OriDiffuse = ((Material*)(*iter))->GetDiffuse();
                        RGBA OriEmissive = ((Material*)(*iter))->GetEmissive();
                        RGBA OriSpecular = ((Material*)(*iter))->GetSpecular();
                        float OriShininess = ((Material*)(*iter))->GetShininess();

                        float Ambient[4] = { OriAmbient.r, OriAmbient.g, OriAmbient.b, OriAmbient.a };
                        float Diffuse[4] = { OriDiffuse.r, OriDiffuse.g, OriDiffuse.b, OriDiffuse.a };
                        float Emissive[4] = { OriEmissive.r, OriEmissive.g, OriEmissive.b, OriEmissive.a };
                        float Specular[4] = { OriSpecular.r, OriSpecular.g, OriSpecular.b, OriSpecular.a };
                        float Shininess = OriShininess;

                        ImGui::InputFloat3("Ambient", Ambient);
                        ImGui::InputFloat3("Diffuse", Diffuse);
                        ImGui::InputFloat3("Emissive", Emissive);
                        ImGui::InputFloat3("Specular", Specular);
                        ImGui::InputFloat("Shininess", &Shininess);

                        ((Material*)(*iter))->SetAmbient(Ambient[0], Ambient[1], Ambient[2], Ambient[3]);
                        ((Material*)(*iter))->SetDiffuse(Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
                        ((Material*)(*iter))->SetEmissive(Emissive[0], Emissive[1], Emissive[2], Emissive[3]);
                        ((Material*)(*iter))->SetSpecular(Specular[0], Specular[1], Specular[2], Specular[3]);
                        ((Material*)(*iter))->SetShininess(Shininess);
                    }

                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    if (showAddComponent)
    {
        std::string WindowName = "Add Component : " + currentShowInfoObject;

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(WindowName.c_str(), &showAddComponent);

        if (ImGui::IsRootWindowOrAnyChildFocused())
            InputMgr->SetFocusWindow(false);
        else
            InputMgr->SetFocusWindow(true);

        const char* listbox_items[] = { "StaticMesh", "Collision", "SkinnedMesh", "Camera", "Material", "TrailRenderer", "BillBoard", "SpriteBillBoard" };
        static int listbox_item_current = 0;
        ImGui::ListBox("Component\nTypes\n", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

        switch (listbox_item_current) {
        case 0:
        {
            auto Meshes = ResourceMgr->ExistFiles(".\\Resource\\Mesh\\*");

            const int Size = Meshes.size();
            char** ComboBoxItems = new char*[Size];

            int i = 0;
            for each(auto item in Meshes)
            {
                ComboBoxItems[i] = new char[64];
                strcpy(ComboBoxItems[i], Meshes[i].c_str());
                i++;
            }

            static int CurItem = 0;
            ImGui::Combo("Meshes", &CurItem, ComboBoxItems, Size);
            
            if (ImGui::Button("Add Component"))
            {
                std::string fullpath = "";
                if (!ResourceMgr->ExistFile(ComboBoxItems[CurItem], &fullpath))
                    break;

                StaticMesh* Mesh = new StaticMesh();
                Mesh->SetFileAddress(ComboBoxItems[CurItem]);
                Mesh->LoadContent();
                auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                Object->AddComponent(Mesh);

                showAddComponent = false;
            }
        }
        break;

        case 1:
        {
            char* ComboBoxItems[] = { "AABB", "SPHERE", "OBB" };
            
            static int CurItem = 0;
            ImGui::Combo("Meshes", &CurItem, ComboBoxItems, 3);

            if (ImGui::Button("Add Component"))
            {
                auto Object = SceneMgr->CurrentScene()->FindObjectByName(currentShowInfoObject);
                
                Collision* Coll = new Collision(Object);
                Coll->ComputeBoundingBox(ResourceMgr->LoadMesh(((StaticMesh*)Object->GetComponent("StaticMesh"))->GetFileAddress())->mesh);
                Coll->LoadContent();
                
                Object->AddComponent(Coll);

                showAddComponent = false;
            }
        }
        break;
        }

        ImGui::End();
    }
}