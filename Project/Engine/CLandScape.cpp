#include "pch.h"
#include "CLandScape.h"

#include "CKeyMgr.h"
#include "CRenderMgr.h"

#include "CTransform.h"
#include "CCamera.h"

#include "CStructuredBuffer.h"

CLandScape::CLandScape()
    : CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
      , m_FaceX(1)
      , m_FaceZ(1)
      , m_Out{}
      , m_BrushScale(Vec2(0.25f, 0.25f))
      , m_BrushIdx(0)
      , m_Mode(NONE)
{
}

CLandScape::~CLandScape()
{
    if (nullptr != m_RaycastOut)
        delete m_RaycastOut;

    if (nullptr != m_WeightMap)
        delete m_WeightMap;
}

void CLandScape::FinalTick()
{
    // ��� ��ȯ
    if (KEY_TAP(KEY::NUM_6))
    {
        if (HEIGHTMAP == m_Mode)
            m_Mode = SPLATING;
        else if (SPLATING == m_Mode)
            m_Mode = NONE;
        else
            m_Mode = HEIGHTMAP;
    }

    // �귯�� �ٲٱ�
    if (KEY_TAP(KEY::NUM_7))
    {
        ++m_BrushIdx;
        if (m_vecBrush.size() <= m_BrushIdx)
            m_BrushIdx = 0;
    }

    // ����ġ �ε��� �ٲٱ�
    if (KEY_TAP(KEY::NUM_8))
    {
        ++m_WeightIdx;
        if (m_ColorTex->GetArraySize() <= static_cast<UINT>(m_WeightIdx))
            m_WeightIdx = 0;
    }

    if (NONE == m_Mode)
        return;

    if (Raycasting() && KEY_PRESSED(KEY::LBTN))
    {
        if (HEIGHTMAP == m_Mode)
        {
            if (nullptr != m_HeightMap)
            {
                // ���̸� ����
                m_HeightMapCS->SetBrushPos(m_RaycastOut);
                m_HeightMapCS->SetBrushScale(m_BrushScale);
                m_HeightMapCS->SetHeightMap(m_HeightMap);
                m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
                m_HeightMapCS->Execute();
            }
        }

        else if (SPLATING == m_Mode)
        {
            if (m_WeightWidth != 0 && m_WeightHeight != 0)
            {
                m_WeightMapCS->SetBrushPos(m_RaycastOut);
                m_WeightMapCS->SetBrushScale(m_BrushScale);
                m_WeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
                m_WeightMapCS->SetWeightMap(m_WeightMap);
                m_WeightMapCS->SetWeightIdx(m_WeightIdx);
                m_WeightMapCS->SetWeightMapWidthHeight(m_WeightWidth, m_WeightHeight);
                m_WeightMapCS->Execute();
            }
        }
    }
}

void CLandScape::Render()
{
    Transform()->Binding();

    GetMaterial(0)->SetScalarParam(INT_0, m_FaceX); // ������ �� ����
    GetMaterial(0)->SetScalarParam(INT_1, m_FaceZ); // ������ �� ����
    GetMaterial(0)->SetScalarParam(INT_2, static_cast<int>(m_Mode)); // ���� ���

    GetMaterial(0)->SetTexParam(TEX_0, m_HeightMap); // ���� ���̸�

    GetMaterial(0)->SetTexParam(TEX_ARR_0, m_ColorTex); // ���� ���� �ؽ���
    GetMaterial(0)->SetTexParam(TEX_ARR_1, m_NormalTex); // ���� �븻 �ؽ���
    GetMaterial(0)->SetScalarParam(INT_3, static_cast<int>(m_ColorTex->GetArraySize()));
    // �ؽ��� �迭 ����

    GetMaterial(0)->SetTexParam(TEX_1, m_vecBrush[m_BrushIdx]); // Brush �ؽ���
    GetMaterial(0)->SetScalarParam(VEC2_0, m_BrushScale); // Brush ũ��
    GetMaterial(0)->SetScalarParam(VEC2_1, m_Out.Location); // LayCast ��ġ
    GetMaterial(0)->SetScalarParam(FLOAT_0, static_cast<float>(m_Out.Success)); // LayCast ��������

    GetMaterial(0)->SetScalarParam(VEC2_2, Vec2(m_WeightWidth, m_WeightHeight)); // ����ġ �ػ�

    m_WeightMap->Binding(20); // WeightMap t20 ���ε�
    GetMaterial(0)->Binding(); // ���� ���ε�

    GetMesh()->Render(0); // ������

    m_WeightMap->Clear(20); // WeightMap ���� ���ε� Ŭ����
}

void CLandScape::SetFace(UINT _X, UINT _Z)
{
    if (m_FaceX == _X && m_FaceZ == _Z)
        return;

    m_FaceX = _X;
    m_FaceZ = _Z;

    CreateMesh();
}


int CLandScape::Raycasting()
{
    // ���� ���� ī�޶� ��������
    CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
    if (nullptr == pCam)
        return false;

    // ����ȭ���� Ŭ����
    m_Out = {};
    m_Out.Distance = 0xffffffff;
    m_RaycastOut->SetData(&m_Out);

    // ī�޶� �������� ���콺�� ���ϴ� Ray ������ ������
    tRay ray = pCam->GetRay();

    // LandScape �� WorldInv ��� ������
    const Matrix& matWorldInv = Transform()->GetWorldInvMat();

    // ���� ���� Ray ������ LandScape �� Local �������� ������
    ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
    ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
    ray.vDir.Normalize();

    // Raycast ��ǻƮ ���̴��� �ʿ��� ������ ����
    m_RaycastCS->SetRayInfo(ray);
    m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
    m_RaycastCS->SetOutBuffer(m_RaycastOut);
    m_RaycastCS->SetHeightMap(m_HeightMap);

    // ��ǻƮ���̴� ����
    m_RaycastCS->Execute();

    // ��� Ȯ��
    m_RaycastOut->GetData(&m_Out);

    return m_Out.Success;
}

void CLandScape::SaveComponent(FILE* _File)
{
}

void CLandScape::LoadComponent(FILE* _File)
{
}
