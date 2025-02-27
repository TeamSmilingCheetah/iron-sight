#include "pch.h"
#include "Runtime/Public/Component/Rendering/CLandScape.h"
#include "Runtime/Public/Component/Camera/CCamera.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CKeyMgr.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	  , m_FaceX(1)
	  , m_FaceZ(1)
	  , m_Out{}
	  , m_BrushScale(Vec2(0.25f, 0.25f))
	  , m_BrushIdx(0)
	  , m_Mode(NONE)
	  , m_RaycastOut()
	  , m_WeightHeight(0)
	  , m_WeightIdx(0)
	  , m_WeightMap()
	  , m_WeightWidth()
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
	// 모드 전환
	if (KEY_TAP(KEY::NUM_6))
	{
		if (HEIGHTMAP == m_Mode)
			m_Mode = SPLATING;
		else if (SPLATING == m_Mode)
			m_Mode = NONE;
		else
			m_Mode = HEIGHTMAP;
	}

	// 브러쉬 바꾸기
	if (KEY_TAP(KEY::NUM_7))
	{
		++m_BrushIdx;
		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}

	// 가중치 인덱스 바꾸기
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
				// 높이맵 설정
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

	GetMaterial(0)->SetScalarParam(INT_0, m_FaceX); // 지형의 면 개수
	GetMaterial(0)->SetScalarParam(INT_1, m_FaceZ); // 지형의 면 개수
	GetMaterial(0)->SetScalarParam(INT_2, static_cast<int>(m_Mode)); // 지형 모드

	GetMaterial(0)->SetTexParam(TEX_0, m_HeightMap); // 지형 높이맵

	GetMaterial(0)->SetTexParam(TEX_ARR_0, m_ColorTex); // 지형 색상 텍스쳐
	GetMaterial(0)->SetTexParam(TEX_ARR_1, m_NormalTex); // 지형 노말 텍스쳐
	GetMaterial(0)->SetScalarParam(INT_3, static_cast<int>(m_ColorTex->GetArraySize()));
	// 텍스쳐 배열 개수

	GetMaterial(0)->SetTexParam(TEX_1, m_vecBrush[m_BrushIdx]); // Brush 텍스쳐
	GetMaterial(0)->SetScalarParam(VEC2_0, m_BrushScale); // Brush 크기
	GetMaterial(0)->SetScalarParam(VEC2_1, m_Out.Location); // LayCast 위치
	GetMaterial(0)->SetScalarParam(FLOAT_0, static_cast<float>(m_Out.Success)); // LayCast 성공여부

	GetMaterial(0)->SetScalarParam(VEC2_2, Vec2(static_cast<float>(m_WeightWidth), static_cast<float>(m_WeightHeight)));
	// 가중치 해상도

	m_WeightMap->Binding(20); // WeightMap t20 바인딩
	GetMaterial(0)->Binding(); // 재질 바인딩

	GetMesh()->Render(0); // 렌더링

	m_WeightMap->Clear(20); // WeightMap 버퍼 바인딩 클리어
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
	// 현재 시점 카메라 가져오기
	CCamera* pCam = CRenderMgr::GetInst()->GetMainCamera();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out);

	return m_Out.Success;
}

void CLandScape::SaveComponent(FILE* _File)
{
}

void CLandScape::LoadComponent(FILE* _File)
{
}
