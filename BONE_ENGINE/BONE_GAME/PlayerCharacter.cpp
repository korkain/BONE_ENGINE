#include "PlayerCharacter.h"

void PlayerCharacter::Init()
{
    gameObject->SetPriority(1);
    gameObject->SetTag("Player");

    W_Key = false;
    S_Key = false;
    A_Key = false;
    D_Key = false;
    
    Sneaking_Key = false;
    Attack_Key = false;
    isSneaking = false;

    speed = 1000;
}

void PlayerCharacter::Reference()
{
    cameraObject = new GameObject();
    cameraObject->SetPriority(1);
    
    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vec3(0, 30, 15) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    cameraObject->AddComponent(tr);
    cameraTr = tr;

    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.5f);
    camera->SetTargetPosition(((Transform3D*)gameObject->transform3D)->GetPosition() + Vec3(0, 30, 0));
    cameraObject->AddComponent(camera);
    mainCamera = camera;

    skinnedMesh = ((SkinnedMesh*)gameObject->GetComponent("SkinnedMesh"));
    CUR_SCENE->AddObject(cameraObject, "MainCamera");

    sword = new GameObject();
    sword->SetPriority(1);

    Transform3D* SwordTransform = new Transform3D();
    auto SwordMatrix = skinnedMesh->GetBoneMatrix("spine_03"); //hand_r
    SwordTransform->CombineMatrix(SwordMatrix);
    SwordTransform->SetPosition(25, -4, 8); 
    SwordTransform->SetRotate(-1.29f, 1.55f, 1.81f); // -0.45f, 0.96f, 1.85f;
    SwordTransform->SetScale(2.0f, 2.0f, 2.0f);
    sword->AddComponent(SwordTransform);
    swordTr = SwordTransform;

    StaticMesh* SwordMesh = new StaticMesh();
    SwordMesh->SetFile("Sword.X");
    sword->AddComponent(SwordMesh);

    sword->AttachParent(this->gameObject);

    CUR_SCENE->AddObject(sword, "PlayerSword");
    sword->SetActive(false);

    shield = new GameObject();
    shield->SetPriority(1);

    Transform3D* ShieldTransform = new Transform3D();
    auto ShieldMatrix = skinnedMesh->GetBoneMatrix("spine_03");
    ShieldTransform->CombineMatrix(ShieldMatrix);
    ShieldTransform->SetPosition(-12.0f, 0, 18.0f);
    ShieldTransform->SetRotate(0.02f , -1.44f, -0.37f);
    ShieldTransform->SetScale(2.0f, 2.0f, 2.0f);
    shield->AddComponent(ShieldTransform);
    shieldTr = ShieldTransform;
    
    StaticMesh* ShieldMesh = new StaticMesh();
    ShieldMesh->SetFile("Shield.X");
    shield->AddComponent(ShieldMesh);

    shield->AttachParent(this->gameObject);

    CUR_SCENE->AddObject(shield, "PlayerShield");
    shield->SetActive(false);

    gui = (PlayerGUI*)(CUR_SCENE->FindObjectByName("GameManager")->GetComponent("PlayerGUI"));

    rigidBody = GET_RIGIDBODY(this->gameObject);
    transform = GET_TRANSFORM_3D(this->gameObject);

    isCombat = false;
    wearItem = false;
}

void PlayerCharacter::Update()
{
    if (isEvent)
    {
        gui->ShowGUI(false);

        rigidBody->SetLinearVelocity(0, 0, 0);
        return;
    }
    
    float rotateYAngle = transform->GetRotateAngle().y;
    rigidBody->SetTransform(transform->GetPosition(), Vec3(0, rotateYAngle, 0));

    gui->ShowGUI(true);


    if (wearItem == false)
    {
        auto Item = CUR_SCENE->FindObjectByName("Sword");

        auto ItemPos = GET_TRANSFORM_3D(Item)->GetPosition();
        auto Pos = transform->GetPosition();

        if (Pos.x >= ItemPos.x - 30 && Pos.x <= ItemPos.x + 30
            && Pos.z >= ItemPos.z - 30 && Pos.z <= ItemPos.z + 30)
        {
            gui->ShowGetItem(true);

            if (InputMgr->KeyDown('F', true))
            {
                SoundMgr->Play2D("GetItem.mp3", 0.2f, false);
                wearItem = true;
                WearItem();
                gui->ShowGetItem(false);
            }
        }
        else
            gui->ShowGetItem(false);
    }

    
    bool Input = false;

    if (InputMgr->KeyDown('R', true) && wearItem)
    {
        if (isCombat)
        {
            isCombat = false;
            gui->SetStatus(PlayerGUI::PLAYER_STATUS::NORMAL);
            NormalMode();
        }
        else
        {
            isCombat = true;
            gui->SetStatus(PlayerGUI::PLAYER_STATUS::COMBAT);
            CombatMode();
        }
    }

    if (InputMgr->KeyDown(VK_CONTROL, true) && !isRun)
    {
        if (isSneaking == false)
        {
            gui->SetStatus(PlayerGUI::PLAYER_STATUS::SNEAKING);
            isSneaking = true;
            speed = 500;
        }
        else
        {
            if (isCombat)
                gui->SetStatus(PlayerGUI::PLAYER_STATUS::COMBAT);
            else
                gui->SetStatus(PlayerGUI::PLAYER_STATUS::NORMAL);

            speed = 1000;
            isSneaking = false;
        }
    }

    if (InputMgr->KeyDown(VK_SHIFT, false))
    {
        gui->SetStatus(PlayerGUI::PLAYER_STATUS::NORMAL);
        NormalMode();
        isCombat = false;

        isRun = true;
        isSneaking = false;
        speed = 2000;
    }
    else if (!isSneaking)
    {
        isRun = false;
        speed = 1000;
    }

    if (InputMgr->KeyDown('W', false) && !Attack_Key)
    {
        if (!SoundMgr->IsPlaying2D("footstep.mp3"))
            SoundMgr->Play2D("footstep.mp3", 0.07f, true);

        Input = true;

        if (isRun)
            skinnedMesh->SetAnimation("Skeleton_Run");
        else if (isSneaking)
            skinnedMesh->SetAnimation("Skeleton_Sneaking");
        else
            skinnedMesh->SetAnimation("Skeleton_1H_walk");
        
        Vec3 Forward = GET_TRANSFORM_3D(gameObject)->GetForward() * SceneMgr->GetTimeDelta() * speed;

        rigidBody->SetLinearVelocity(Forward.x, Forward.y, -Forward.z);

        W_Key = true;
    }
    else if (W_Key)
    {
        rigidBody->SetLinearVelocity(0, 0, 0);
        W_Key = false;
    }

    if (InputMgr->KeyDown('S', false) && !Attack_Key &&!isSneaking)
    {
        if (!SoundMgr->IsPlaying2D("footstep.mp3"))
            SoundMgr->Play2D("footstep.mp3", 0.07f, true);

        Input = true;

        if (isSneaking)
            skinnedMesh->SetAnimation("Skeleton_Sneaking");
        else
            skinnedMesh->SetAnimation("Skeleton_walking_back");
        
        Vec3 Backword = -GET_TRANSFORM_3D(gameObject)->GetForward() * SceneMgr->GetTimeDelta() * 500;

        rigidBody->SetLinearVelocity(Backword.x, Backword.y, -Backword.z);

        S_Key = true;
    }
    else if (S_Key)
    {
        rigidBody->SetLinearVelocity(0, 0, 0);
        S_Key = false;
    }

    if (InputMgr->KeyDown('A', false) && !Attack_Key)
    {
        Input = true;
        A_Key = true;

        rigidBody->SetAngularVelocity(0, -1.5f, 0);
    }
    else if (A_Key == true)
    {
        rigidBody->SetAngularVelocity(0, 0, 0);
        A_Key = false;
    }
        
    if (InputMgr->KeyDown('D', false) && !Attack_Key)
    {
        Input = true;
        D_Key = true;

        rigidBody->SetAngularVelocity(0, 1.5f, 0);
    }
    else if (D_Key == true)
    {
        rigidBody->SetAngularVelocity(0, 0, 0);
        D_Key = false;
    }

    if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN && !Sneaking_Key && wearItem)
    {
        if (gui->GetStatus() == PlayerGUI::PLAYER_STATUS::NORMAL)
        {
            isCombat = true;
            CombatMode();
            gui->SetStatus(PlayerGUI::PLAYER_STATUS::COMBAT);
        }

        Input = true;
        Attack_Key = true;

        skinnedMesh->SetAnimation("Skeleton_1H_swing_left");
        skinnedMesh->SetAnimationLoop(false);
    }
    else if (Attack_Key == true)
    {
        if (skinnedMesh->GetAnimationRate() >= 0.99f)
            Attack_Key = false;
        else
            Input = true;
    }

    if (Input == false)
    {
        SoundMgr->Stop2D("footstep.mp3");

        skinnedMesh->SetAnimationLoop(true);

        if (isCombat)
            skinnedMesh->SetAnimation("Skeleton_1H_combat_mode");
        else if (isSneaking)
            skinnedMesh->SetAnimation("Skeleton_Crouching");
        else
            skinnedMesh->SetAnimation("Skeleton_Idle");
    }
}

void PlayerCharacter::WearItem()
{
    wearItem = true;

    sword->SetActive(true);
    shield->SetActive(true);
}

bool PlayerCharacter::IsWoreItem()
{
    return wearItem;
}

void PlayerCharacter::CombatMode()
{
    auto SwordMatrix = skinnedMesh->GetBoneMatrix("hand_r");
    swordTr->CombineMatrix(SwordMatrix);
    swordTr->SetPosition(0, 0, 0);
    swordTr->SetRotate(-0.45f, 0.96f, 1.85f);

    //auto ShieldMatrix = skinnedMesh->GetBoneMatrix("spine_03");
    //shieldTr->CombineMatrix(ShieldMatrix);
    //shieldTr->SetPosition(-12.0f, 0, 18.0f);
    //shieldTr->SetRotate(0.02f, -1.44f, -0.37f);
    //shieldTr->SetScale(2.0f, 2.0f, 2.0f);
}

void PlayerCharacter::NormalMode()
{
    auto SwordMatrix = skinnedMesh->GetBoneMatrix("spine_03"); //hand_r
    swordTr->CombineMatrix(SwordMatrix);
    swordTr->SetPosition(25, -4, 8);
    swordTr->SetRotate(-1.29f, 1.55f, 1.81f); // -0.45f, 0.96f, 1.85f;
}

void PlayerCharacter::LateUpdate()
{
    if (isEvent)
        return;
     
    POINT pt;
    float fDelta = 0.001f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.

    GetCursorPos(&pt);
    int dx = pt.x - mouseX;
    int dy = pt.y - mouseY;
                          
    Vec3 Target = GET_TRANSFORM_3D(gameObject)->GetPosition() + Vec3(0, 40, 0);
    Vec3 Eye = GET_TRANSFORM_3D(cameraObject)->GetPosition();

    Vec3 Dir = Eye - Target;

    Vec3 YRotatePos = Target;

    Matrix YMat;
    D3DXMatrixRotationY(&YMat, dx * fDelta);
    D3DXVec3TransformCoord(&YRotatePos, &Dir, &YMat);
        
    Vec3 Cross;
    D3DXVec3Normalize(&Dir, &Dir);
    D3DXVec3Cross(&Cross, &Dir, &Vec3(0, 1, 0));
    D3DXVec3Normalize(&Cross, &Cross);

    Vec3 FinalPos;

    Matrix CrossMat;
    D3DXMatrixRotationAxis(&CrossMat, &Cross, dy * fDelta);
    D3DXVec3TransformCoord(&FinalPos, &YRotatePos, &CrossMat);
    D3DXVec3Normalize(&FinalPos, &FinalPos);
    FinalPos *= 15;

    pt.x = (RenderMgr->GetWidth()) / 2;
    pt.y = (RenderMgr->GetHeight()) / 2;
                      
    SetCursorPos(pt.x, pt.y);
    mouseX = pt.x;
    mouseY = pt.y;
    
    mainCamera->SetTargetPosition(Target);
    cameraTr->SetPosition(FinalPos + Target);

    mainCamera->FixedUpdate(cameraObject);
}