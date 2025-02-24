#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/CTaskMgr.h>
#include "CMissileScript.h"

CPlayerScript::CPlayerScript()
    : CScript(static_cast<UINT>(SCRIPT_TYPE::PLAYERSCRIPT))
      , m_PlayerSpeed(500)
      , m_PaperBurnIntence(0.f)
{
    AddScriptParam(tScriptParam{SCRIPT_PARAM::FLOAT, "Player Speed", &m_PlayerSpeed});
    AddScriptParam(tScriptParam{SCRIPT_PARAM::TEXTURE, "Test Texture", &m_TargetTex});
    AddScriptParam(tScriptParam{SCRIPT_PARAM::PREFAB, "Missile", &m_Prefab});
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
    //m_Prefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\Tile.pref", L"Prefab\\Tile.pref");
}

void CPlayerScript::Tick()
{
    Vec3 vPos = Transform()->GetRelativePos();

    if (KEY_PRESSED(KEY::LEFT))
        vPos.x -= m_PlayerSpeed * DT;
    if (KEY_PRESSED(KEY::RIGHT))
        vPos.x += m_PlayerSpeed * DT;
    if (KEY_PRESSED(KEY::UP))
        vPos.y += m_PlayerSpeed * DT;
    if (KEY_PRESSED(KEY::DOWN))
        vPos.y -= m_PlayerSpeed * DT;

    if (KEY_PRESSED(KEY::Z))
    {
        Vec3 vRot = Transform()->GetRelativeRotation();
        vRot.z += DT * XM_PI;
        Transform()->SetRelativeRotation(vRot);
        vPos.z += DT * 100.f;

        /*Vec3 vScale = Transform()->GetRelativeScale();
        vScale.x += 200.f * DT;
        vScale.y += 200.f * DT;
        Transform()->SetRelativeScale(vScale);*/
    }

    if (KEY_PRESSED(KEY::NUM_0))
    {
        Vec3 vScale = Transform()->GetRelativeScale();
        vScale.x += DT * 0.1f;
        vScale.y += DT * 0.1f;
        Transform()->SetRelativeScale(vScale);
    }

    if (KEY_PRESSED(KEY::NUM_9))
    {
        DrawDebugRect(Vec4(0.f, 1.f, 0.f, 0.5f), Transform()->GetRelativePos()
                      , Vec2(200.f, 200.f), Vec3(0.f, 0.f, 0.f), true, 0.f);
    }

    Transform()->SetRelativePos(vPos);

    // 미사일 발사
    if (KEY_TAP(KEY::SPACE))
    {
        //DrawDebugCube(Vec4(0.f, 1.f, 0.f, 1.f), vPos, Vec3(100.f, 100.f, 100.f), Vec3(0.f, 0.f, 0.f), false, 3.f);
        //DrawDebugSphere(Vec4(0.f, 1.f, 0.f, 1.f), vPos, 100.f, true, 5.f);
        DrawDebugLine(Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 10000.f, 0.f), true,
                      10.f);
    }
}

void CPlayerScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
                                 CCollider2D* _OtherCollider)
{
}

void CPlayerScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObject,
                            CCollider2D* _OtherCollider)
{
}

void CPlayerScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject,
                               CCollider2D* _OtherCollider)
{
}

void CPlayerScript::SaveComponent(FILE* _File)
{
    fwrite(&m_PlayerSpeed, sizeof(float), 1, _File);
    fwrite(&m_PaperBurnIntence, sizeof(float), 1, _File);
    SaveAssetRef(m_TargetTex, _File);
}

void CPlayerScript::LoadComponent(FILE* _File)
{
    fread(&m_PlayerSpeed, sizeof(float), 1, _File);
    fread(&m_PaperBurnIntence, sizeof(float), 1, _File);
    LoadAssetRef(m_TargetTex, _File);
}
