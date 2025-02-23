#include "pch.h"
#include "CMissileScript.h"


CMissileScript::CMissileScript()
    : CScript(static_cast<UINT>(SCRIPT_TYPE::CAMERASCRIPT))
      , m_Velocity(Vec3(0.f, 500.f, 0.f))
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::Begin()
{
    // CAssetMgr �� ������ �����Ų ������ ����
    Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl");
    MeshRender()->SetMaterial(pMtrl, 0);
    MeshRender()->GetDynamicMaterial(0);

    Ptr<CTexture> pMissileTex = CAssetMgr::GetInst()->Load<CTexture>(
        L"Missile", L"Texture\\missile.png");
    MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pMissileTex);
}

void CMissileScript::Tick()
{
    Vec3 vWorldPos = Transform()->GetRelativePos();

    vWorldPos += m_Velocity * DT;

    Transform()->SetRelativePos(vWorldPos);
}

void CMissileScript::SaveComponent(FILE* _File)
{
    fwrite(&m_Velocity, sizeof(float), 1, _File);
}

void CMissileScript::LoadComponent(FILE* _File)
{
    fread(&m_Velocity, sizeof(float), 1, _File);
}
