#include "pch.h"
#include "Runtime/Public/Component/Camera/CFrustum.h"
#include "Runtime/Public/Component/Camera/CCamera.h"

CFrustum::CFrustum()
    : m_Owner(nullptr)
{
    // 투영 좌표계 최대 시야 범위
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

CFrustum::~CFrustum() = default;

void CFrustum::FinalTick()
{
    const Matrix& matViewInv = m_Owner->GetViewInvMat();
    const Matrix& matProjInv = m_Owner->GetProjInvMat();

    Matrix matInv = matProjInv * matViewInv;

    // 월드 공간에서 카메라 시야 끝트머리 8개 정점 위치 구하기
    Vec3 vWorldPos[8] = {};
    for (int i = 0; i < 8; ++i)
    {
        vWorldPos[i] = XMVector3TransformCoord(m_ProjPos[i], matInv);
    }

    // 투영 좌표계 최대 시야 범위
    //     4 ---- 5
    //    /|     /|
    //   / |    / |
    //  0 ---- 1  6
    //  | /    | /
    //  |/	   |/
    //  3 ---- 2
    m_Face[static_cast<int>(FACE_TYPE::FT_NEAR)] = XMPlaneFromPoints(vWorldPos[0], vWorldPos[1], vWorldPos[2]);
    m_Face[static_cast<int>(FACE_TYPE::FT_FAR)] = XMPlaneFromPoints(vWorldPos[5], vWorldPos[4], vWorldPos[7]);
    m_Face[static_cast<int>(FACE_TYPE::FT_RIGHT)] = XMPlaneFromPoints(vWorldPos[1], vWorldPos[5], vWorldPos[6]);
    m_Face[static_cast<int>(FACE_TYPE::FT_LEFT)] = XMPlaneFromPoints(vWorldPos[7], vWorldPos[4], vWorldPos[0]);
    m_Face[static_cast<int>(FACE_TYPE::FT_TOP)] = XMPlaneFromPoints(vWorldPos[4], vWorldPos[5], vWorldPos[1]);
    m_Face[static_cast<int>(FACE_TYPE::FT_BOT)] = XMPlaneFromPoints(vWorldPos[7], vWorldPos[2], vWorldPos[6]);
}

/**
 * @brief 단일 지점이 frustum 내에 위치하는지 판별하는 함수
 *
 * @param PWorldPos world 좌표
 * @return 해당 좌표가 frustum 내에 있는지 판별한 결과를 반환
 */
bool CFrustum::IsInFrustum(const Vec3& PWorldPos) const
{
    // Frustum 판정에 여유 공간 추가
    constexpr float FRUSTUM_TOLERANCE = 0.1f;

	for (auto Face : m_Face)
	{
		auto Normal = Vec3(Face.x, Face.y, Face.z);

		if (Normal.Dot(PWorldPos) + Face.w > FRUSTUM_TOLERANCE)
		{
			return false;
		}
	}

    return true;
}
