#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"


CFrustum::CFrustum()
    : m_Owner(nullptr)
{
    // ���� ��ǥ�� �ִ� �þ� ���� 
    //     4 ---- 5
    //    /|     /|
    //   / |    / |
    //  0 ---- 1  6
    //  |      | /
    //  |	   |/
    //  3 ---- 2
    m_ProjPos[0] = Vec3(-1.f, 1.f, 0.f);
    m_ProjPos[1] = Vec3(1.f, 1.f, 0.f);
    m_ProjPos[2] = Vec3(1.f, -1.f, 0.f);
    m_ProjPos[3] = Vec3(-1.f, -1.f, 0.f);

    m_ProjPos[4] = Vec3(-1.f, 1.f, 1.f);
    m_ProjPos[5] = Vec3(1.f, 1.f, 1.f);
    m_ProjPos[6] = Vec3(1.f, -1.f, 1.f);
    m_ProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::FinalTick()
{
    const Matrix& matViewInv = m_Owner->GetViewInvMat();
    const Matrix& matProjInv = m_Owner->GetProjInvMat();

    Matrix matInv = matProjInv * matViewInv;

    // ���� �������� ī�޶� �þ� ��Ʈ�Ӹ� 8�� ���� ��ġ ���ϱ�
    Vec3 vWorldPos[8] = {};
    for (int i = 0; i < 8; ++i)
    {
        vWorldPos[i] = XMVector3TransformCoord(m_ProjPos[i], matInv);
    }


    // ���� ��ǥ�� �ִ� �þ� ���� 
    //     4 ---- 5
    //    /|     /|
    //   / |    / |
    //  0 ---- 1  6
    //  | /    | /
    //  |/	   |/
    //  3 ---- 2
    m_Face[FT_NEAR] = XMPlaneFromPoints(vWorldPos[0], vWorldPos[1], vWorldPos[2]);
    m_Face[FT_FAR] = XMPlaneFromPoints(vWorldPos[5], vWorldPos[4], vWorldPos[7]);
    m_Face[FT_RIGHT] = XMPlaneFromPoints(vWorldPos[1], vWorldPos[5], vWorldPos[6]);
    m_Face[FT_LEFT] = XMPlaneFromPoints(vWorldPos[7], vWorldPos[4], vWorldPos[0]);
    m_Face[FT_TOP] = XMPlaneFromPoints(vWorldPos[4], vWorldPos[5], vWorldPos[1]);
    m_Face[FT_BOT] = XMPlaneFromPoints(vWorldPos[7], vWorldPos[2], vWorldPos[6]);
}

bool CFrustum::FrustumCheck(Vec3 _WorldPos)
{
    for (int i = 0; i < FT_END; ++i)
    {
        auto vNormal = Vec3(m_Face[i].x, m_Face[i].y, m_Face[i].z);

        // ax + by + cz + d > 0 
        if (vNormal.Dot(_WorldPos) + m_Face[i].w > 0)
        {
            return false;
        }
    }

    return true;
}

bool CFrustum::FrustumCheckSphere(Vec3 _WorldCenter, float _Radius)
{
    for (int i = 0; i < FT_END; ++i)
    {
        auto vNormal = Vec3(m_Face[i].x, m_Face[i].y, m_Face[i].z);

        // ax + by + cz + d > r 
        if (vNormal.Dot(_WorldCenter) + m_Face[i].w > _Radius)
        {
            return false;
        }
    }

    return true;
}
