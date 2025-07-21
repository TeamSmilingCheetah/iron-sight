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
	  , m_RaycastOut()
	  , m_Out{}
	  , m_BrushScale(Vec2(0.25f, 0.25f))
	  , m_BrushIdx(0)
	  , m_WeightMap()
	  , m_WeightWidth()
	  , m_WeightHeight(0)
	  , m_WeightIdx(0)
	  , m_RayCollisionOut(nullptr)
      , m_Mode(NONE)
{
}

CLandScape::~CLandScape()
{
	delete m_RaycastOut;
	delete m_WeightMap;
	delete m_RayCollisionOut;
}

void CLandScape::ColisionRayStack(void* PRayObject, const tRay& PRayPosDir)
{
	m_vecRayColInst.push_back(tRayCollision(PRayObject, PRayPosDir));
}

void CLandScape::FinalTick()
{
	m_vecRayColInst.clear();

	// 모드 전환
	if (KEY_TAP(KEY::NUMPAD_6))
	{
		if (HEIGHTMAP == m_Mode)
			m_Mode = SPLATING;
		else if (SPLATING == m_Mode)
			m_Mode = NONE;
		else
			m_Mode = HEIGHTMAP;
	}

	// 브러쉬 바꾸기
	if (KEY_TAP(KEY::NUMPAD_7))
	{
		++m_BrushIdx;
		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}

	// 가중치 인덱스 바꾸기
	if (KEY_TAP(KEY::NUMPAD_8))
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

				m_HeightMap->CaptureTextureCustom(m_CachedHeightData);
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

void CLandScape::SetFace(UINT PX, UINT PZ)
{
	if (m_FaceX == PX && m_FaceZ == PZ)
		return;

	m_FaceX = PX;
	m_FaceZ = PZ;

	CreateMesh();
	SetAABB();
}

/**
 * @brief Landscape의 AABB를 산출하는 함수
 */
void CLandScape::SetAABB()
{
	// Landscape는 XZ 평면에 놓여있고, Y는 높이맵에 따라 결정됨
	// 월드 스케일을 반영하여 실제 크기를 계산
	Vec3 WorldScale = Transform()->GetWorldScale();
	float FaceX = static_cast<float>(m_FaceX) * WorldScale.x;
	float FaceZ = static_cast<float>(m_FaceZ) * WorldScale.z;

	Vec3 LocalMin = Vec3(0.f, 0.f, 0.f);
	Vec3 LocalMax = Vec3(FaceX, 0.f, FaceZ);

	// 높이맵이 있다면 Y 범위도 계산
	if (m_HeightMap != nullptr && !m_CachedHeightData.empty())
	{
		float MinHeight = FLT_MAX;
		float MaxHeight = -FLT_MAX;

		// 캐시된 높이 데이터에서 최소/최대 높이 찾기
		for (float Height : m_CachedHeightData)
		{
			MinHeight = min(MinHeight, Height);
			MaxHeight = max(MaxHeight, Height);
		}

		// Min Thickness
		if (fabs(MaxHeight - MinHeight) < 1e-4f)
		{
			MinHeight -= 1.f;
			MaxHeight += 1.f;
		}

		LocalMin.y = MinHeight * WorldScale.y;
		LocalMax.y = MaxHeight * WorldScale.y;
	}
	// Make Min Thickness If No HeightMap
	else
	{
		LocalMin.y = -1.f * WorldScale.y;
		LocalMax.y = 1.f * WorldScale.y;
	}

	Vec3 LocalCorners[8] = {
		Vec3(LocalMin.x, LocalMin.y, LocalMin.z),
		Vec3(LocalMax.x, LocalMin.y, LocalMin.z),
		Vec3(LocalMax.x, LocalMin.y, LocalMax.z),
		Vec3(LocalMin.x, LocalMin.y, LocalMax.z),
		Vec3(LocalMin.x, LocalMax.y, LocalMin.z),
		Vec3(LocalMax.x, LocalMax.y, LocalMin.z),
		Vec3(LocalMax.x, LocalMax.y, LocalMax.z),
		Vec3(LocalMin.x, LocalMax.y, LocalMax.z),
	};

	// Calculate World AABB
	Vec3 WorldCorners[8];
	const Matrix& WorldMatrix = Transform()->GetWorldMat();
	for (int i = 0; i < 8; ++i)
	{
		WorldCorners[i] = XMVector3TransformCoord(LocalCorners[i], WorldMatrix);
	}

	Vec3 WorldMin = WorldCorners[0];
	Vec3 WorldMax = WorldCorners[0];

	for (int i = 1; i < 8; ++i)
	{
		WorldMin.x = min(WorldMin.x, WorldCorners[i].x);
		WorldMin.y = min(WorldMin.y, WorldCorners[i].y);
		WorldMin.z = min(WorldMin.z, WorldCorners[i].z);

		WorldMax.x = max(WorldMax.x, WorldCorners[i].x);
		WorldMax.y = max(WorldMax.y, WorldCorners[i].y);
		WorldMax.z = max(WorldMax.z, WorldCorners[i].z);
	}

	// Save New AABB
	m_AABB = AABB(WorldMin, WorldMax);
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

	m_RayCollisionOut->Create(sizeof(tRayCollision), 1, SRV_UAV, true);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	// m_RaycastCS->SetRayInfo(ray);
	// m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	// m_RaycastCS->SetOutBuffer(m_RaycastOut);
	// m_RaycastCS->SetHeightMap(m_HeightMap);
	//
	// m_RaycastCS->SetRayInOutBuffer(m_RayCollisionOut);
	// m_RaycastCS->SetRayInOutCount(0);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out);

	return m_Out.Success;
}

tRaycastOut CLandScape::ColliderRaycasting(tRay PRay) const
{
	// 구조화버퍼 클리어
	tRaycastOut pRayInfo;
	pRayInfo = {};
	pRayInfo.Distance = 0xffffffff;
	m_RaycastOut->SetData(&pRayInfo);

	m_RayCollisionOut->Create(sizeof(tRayCollision), 1, SRV_UAV, true);

	// 원본 Ray 정보 저장
	tRay WorldRay = PRay;

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const Matrix& matWorld = Transform()->GetWorldMat();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	PRay.vStart = XMVector3TransformCoord(PRay.vStart, matWorldInv);
	PRay.vDir = XMVector3TransformNormal(PRay.vDir, matWorldInv);
	PRay.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	// m_RaycastCS->SetRayInfo(PRay);
	// m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	// m_RaycastCS->SetOutBuffer(m_RaycastOut);
	// m_RaycastCS->SetHeightMap(m_HeightMap);
	//
	// m_RaycastCS->SetRayInOutBuffer(m_RayCollisionOut);
	// m_RaycastCS->SetRayInOutCount(0);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&pRayInfo);

	// 성공 시 거리 계산
	if (pRayInfo.Success)
	{
		// 충돌 위치를 UV에서 로컬 좌표로 변환
		Vec3 localHitPos;
		localHitPos.x = pRayInfo.Location.x * static_cast<float>(m_FaceX);
		localHitPos.z = (1.0f - pRayInfo.Location.y) * static_cast<float>(m_FaceZ);

		// 높이맵에서 y값 가져오기
		localHitPos.y = 0.0f;
		if (m_HeightMap != nullptr)
		{
			// 높이맵 텍스처에서 해당 UV 위치의 높이 값을 가져옴
			UINT heightMapWidth = m_HeightMap->GetWidth();
			UINT heightMapHeight = m_HeightMap->GetHeight();

			// UV 좌표 계산
			float u = pRayInfo.Location.x;
			float v = pRayInfo.Location.y;

			// UV 좌표를 높이맵 텍스처 좌표로 변환 (픽셀 위치)
			UINT x = static_cast<UINT>(u * static_cast<float>(heightMapWidth - 1));
			UINT y = static_cast<UINT>(v * static_cast<float>(heightMapHeight - 1));

			// 높이값 추출
			// 텍스처에서 높이 값 가져오기
			localHitPos.y = m_CachedHeightData[y * heightMapWidth + x];
		}

		// 로컬 히트 위치를 월드로 변환
		Vec3 worldHitPos = XMVector3TransformCoord(localHitPos, matWorld);

		// 월드 공간에서의 거리 계산
		Vec3 worldDist = worldHitPos - WorldRay.vStart;
		pRayInfo.Distance = static_cast<UINT>(worldDist.Length());
	}

	return pRayInfo;
}

vector<tRayCollision>& CLandScape::Collidercalcul()
{
	// 검사할 데이터가 0개 이면 즉시 종료
	if (0 == m_vecRayColInst.size())
		return m_vecRayColInst;

	size_t Count = m_vecRayColInst.size();

	// 구조체 크기에 따라 구조체 버퍼 초기화
	m_RayCollisionOut->Create(sizeof(tRayCollision), static_cast<UINT>(Count), SRV_UAV, true);

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const Matrix& matWorld = Transform()->GetWorldMat();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	for (size_t i = 0; i < m_vecRayColInst.size(); ++i)
	{
		m_vecRayColInst[i].RayPos = XMVector3TransformCoord(m_vecRayColInst[i].RayWorldPos, matWorldInv);
		m_vecRayColInst[i].RayDir = XMVector3TransformNormal(m_vecRayColInst[i].RayDir, matWorldInv);
		m_vecRayColInst[i].RayDir.Normalize();
	}

	// 로컬좌표 기준으로 데이터 전달
	m_RayCollisionOut->SetData(m_vecRayColInst.data());

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	// m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	// m_RaycastCS->SetOutBuffer(m_RaycastOut);
	// m_RaycastCS->SetHeightMap(m_HeightMap);
	//
	// m_RaycastCS->SetRayInOutBuffer(m_RayCollisionOut);
	// m_RaycastCS->SetRayInOutCount(static_cast<UINT>(Count));
	// m_RaycastCS->SetLandWorldMat(matWorld);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RayCollisionOut->GetData(m_vecRayColInst.data());

	return m_vecRayColInst;
}

Vec3 CLandScape::GetWorldPosByLandScape(Vec3 PTargetWorldPos) const
{
	// 랜드스케이프의 월드 행렬
	const Matrix& matWorld = Transform()->GetWorldMat();

	// 역행렬을 이용하여 월드 좌표를 랜드스케이프 로컬 좌표로 변환
	Matrix matWorldInv = Transform()->GetWorldInvMat();
	Vec3 localPos = XMVector3TransformCoord(PTargetWorldPos, matWorldInv);

	// 로컬 좌표를 랜드스케이프 텍스쳐 UV 좌표로 변환
	// 랜드스케이프의 X, Z 크기에 따라 조정
	float u = localPos.x / static_cast<float>(m_FaceX);
	float v = 1.f - (localPos.z / static_cast<float>(m_FaceZ)); // v 좌표는 상하가 반전되어 있으므로 1에서 빼줌

	// 로컬 좌표가 랜드스케이프 범위 내에 있는지 확인
	if (localPos.x < 0.f || localPos.x >static_cast<float>(m_FaceX) ||
		localPos.z < 0.f || localPos.z >static_cast<float>(m_FaceZ))
	{
		// 범위를 벗어나면 큰 음수 값을 반환
		Vec3 FailPos = Vec3(-10000.f, -10000.f, -10000.f);
		return FailPos;
	}

	// 높이맵에서 해당 위치의 높이(Y) 값을 가져옴
	float height = 0.f;
	if (m_HeightMap != nullptr)
	{
		// 높이맵 텍스처에서 해당 UV 위치의 높이 값을 가져옴
		UINT heightMapWidth = m_HeightMap->GetWidth();
		UINT heightMapHeight = m_HeightMap->GetHeight();

		// UV 좌표를 높이맵 텍스처 좌표로 변환 (픽셀 위치)
		UINT x = static_cast<UINT>(u * static_cast<float>(heightMapWidth - 1));
		UINT y = static_cast<UINT>(v * static_cast<float>(heightMapHeight - 1));

		// 높이값 추출
		// 텍스처에서 높이 값 가져오기
		height = m_CachedHeightData[y * heightMapWidth + x];
	}

	// 최종 로컬 좌표에 높이를 적용
	localPos.y = height;

	// 로컬 좌표를 다시 월드 좌표로 변환
	Vec3 worldPos = XMVector3TransformCoord(localPos, matWorld);

	return worldPos;
}

Vec3 CLandScape::GetWorldPosLandNormal(Vec3& PTargetWorldPos) const
{
	// 랜드스케이프의 월드 행렬
	const Matrix& matWorld = Transform()->GetWorldMat();

	// 역행렬을 이용하여 월드 좌표를 랜드스케이프 로컬 좌표로 변환
	Matrix matWorldInv = Transform()->GetWorldInvMat();
	Vec3 localPos = XMVector3TransformCoord(PTargetWorldPos, matWorldInv);

	// 로컬 좌표를 랜드스케이프 텍스쳐 UV 좌표로 변환
	// 랜드스케이프의 X, Z 크기에 따라 조정
	float u = localPos.x / static_cast<float>(m_FaceX);
	float v = 1.f - (localPos.z / static_cast<float>(m_FaceZ)); // v 좌표는 상하가 반전되어 있으므로 1에서 빼줌

	// 로컬 좌표가 랜드스케이프 범위 내에 있는지 확인
	if (localPos.x < 0.f || localPos.x >static_cast<float>(m_FaceX) ||
		localPos.z < 0.f || localPos.z >static_cast<float>(m_FaceZ))
	{
		// 범위를 벗어나면 큰 음수 값을 반환
		Vec3 FailPos = Vec3(-10000.f, -10000.f, -10000.f);
		return FailPos;
	}

	// 높이맵에서 해당 위치의 높이(Y) 값을 가져옴
	// 높이맵이 없으면 그냥 위로 올라가는 노말벡터 반환
	if (m_HeightMap == nullptr)
	{
		return {0.f, 1.f, 0.f};
	}
	// 높이맵 텍스처에서 해당 UV 위치의 높이 값을 가져옴
	UINT heightMapWidth = m_HeightMap->GetWidth();
	UINT heightMapHeight = m_HeightMap->GetHeight();

	// UV 좌표를 높이맵 텍스처 좌표로 변환 (픽셀 위치)
	UINT x = static_cast<UINT>(u * static_cast<float>(heightMapWidth - 1));
	UINT y = static_cast<UINT>(v * static_cast<float>(heightMapHeight - 1));

	// 인접한 4개의 점 위치 계산
	UINT xRight = min(x + 1, heightMapWidth - 1);
	UINT xLeft = 0;
	if (x > 0)
		xLeft = x - 1;
	UINT yUp = min(y + 1, heightMapHeight - 1);
	UINT yDown = 0;
	if (y > 0)
		yDown = y - 1;

	// 높이값 추출
	// 텍스처에서 높이 값 가져오기
	float heightCenter = m_CachedHeightData[y * heightMapWidth + x];
	float heightRight = m_CachedHeightData[y * heightMapWidth + xRight];
	float heightLeft = m_CachedHeightData[y * heightMapWidth + xLeft];
	float heightUp = m_CachedHeightData[yUp * heightMapWidth + x];
	float heightDown = m_CachedHeightData[yDown * heightMapWidth + x];

	// x벡터, z벡터 생성
	Vec3 vecX = Vec3(static_cast<float>(xRight - xLeft) * static_cast<float>(m_FaceX) / static_cast<float>(heightMapWidth - 1),
					heightRight - heightLeft,
					0.0f);

	Vec3 vecZ = Vec3(0.0f,
					heightUp - heightDown,
					static_cast<float>(yUp - yDown) * static_cast<float>(m_FaceZ) / static_cast<float>(heightMapHeight - 1));

	// 두 벡터의 외적으로 노말 벡터 계산
	Vec3 normal = vecZ.Cross(vecX);
	normal.Normalize();

	// 로컬 좌표를 다시 월드 좌표로 변환
	normal = XMVector3TransformNormal(normal, matWorld);
	normal.Normalize();

	localPos.y = heightCenter;
	PTargetWorldPos = XMVector3TransformCoord(localPos, matWorld);

	return normal;
}

void CLandScape::SaveComponent(FILE* PFile)
{
	(void)fwrite(&m_FaceX, sizeof(UINT), 1, PFile);
	(void)fwrite(&m_FaceZ, sizeof(UINT), 1, PFile);

	SaveAssetRef(m_HeightMap, PFile);
	SaveAssetRef(m_ColorTex, PFile);
	SaveAssetRef(m_NormalTex, PFile);
}

void CLandScape::LoadComponent(FILE* PFile)
{
	(void)fread(&m_FaceX, sizeof(UINT), 1, PFile);
	(void)fread(&m_FaceZ, sizeof(UINT), 1, PFile);
	CreateMesh();


	LoadAssetRef(m_HeightMap, PFile);
	LoadAssetRef(m_ColorTex, PFile);
	LoadAssetRef(m_NormalTex, PFile);

	m_HeightMap->CaptureTextureCustom(m_CachedHeightData);
	SetAABB();
}
