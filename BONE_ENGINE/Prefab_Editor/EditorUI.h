#pragma once
#include <Common.h>
#include <SceneManager.h>
using namespace BONE_GRAPHICS;

class EditorUI : public GUI_Scene {
private:
    bool open;
    bool showObjectInfo;
    bool showAddComponent;
    bool showPrefabHierarchical;

    int childSize;

    std::string currentObjectName;
    std::string rootObjectName;

public:
    EditorUI();

    void ShowFileMenu();
    void ShowOpitionMenu();
    void ShowHelpMenu();

    void ShowGameObjectTree(std::string treeName);

    void AllChildCheck(GameObject* parent);

    virtual void UpdateFrame();
};