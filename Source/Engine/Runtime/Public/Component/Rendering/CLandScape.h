#pragma once

#include <array>

#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Rendering/Shader/CHeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CWeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"

using std::array;

struct tRaycastOut
{
	Vec2 Location;
	UINT Distance;
	int Success;
};

struct tRayCollision
{
	void* RayObj;
	Vec3 RayWorldPos;
	Vec3 RayPos;
	Vec3 RayDir;
	float RayLength;
	UINT Distance;
	int Success;
	int Padding[2];

	tRayCollision()
		: RayObj(nullptr)
		  , RayLength(0)
		  , Distance(0xffffffff)
		  , Success(0)
		  , Padding{0}
	{
	}

	tRayCollision(void* PID, const tRay& PRay)
		: RayObj(PID)
		  , RayPos(PRay.vStart)
		  , RayDir(PRay.vDir)
		  , RayLength(0)
		  , Distance(0xffffffff)
		  , Success(0)
		  , Padding{0}
	{
	}
};

enum LANDSCAPE_MODE : UINT8
{
	NONE,
	HEIGHTMAP,
	SPLATING,
};

struct tWeight8
{
	float arrWeight[8];
};

class CLandScape :
	public CRenderComponent
{
	UINT m_FaceX; // 지형 가로 면 개수
	UINT m_FaceZ; // 지형 세로 면 개수

	Ptr<CTexture> m_HeightMap; // 지형에 적용시킬 높이맵
	Ptr<CHeightMapCS> m_HeightMapCS; // 높이맵 수정용 컴퓨트 쉐이더
	vector<float> m_CachedHeightData; // 높이맵 데이터

	// Raycasting
	Ptr<CRaycastCS> m_RaycastCS;
	CStructuredBuffer* m_RaycastOut; // 마우스 픽킹 위치
	tRaycastOut m_Out; // 마우스 픽킹 위치
	vector<tRaycastOut> m_OutCol; // Ray충돌체용

	vector<Ptr<CTexture>> m_vecBrush;
	Vec2 m_BrushScale; // 브러쉬 크기
	UINT m_BrushIdx; // // 브러쉬 인덱스

	// WeightMap
	Ptr<CTexture> m_ColorTex;
	Ptr<CTexture> m_NormalTex;
	CStructuredBuffer* m_WeightMap;
	UINT m_WeightWidth;
	UINT m_WeightHeight;
	Ptr<CWeightMapCS> m_WeightMapCS;
	int m_WeightIdx;

	// Collision연산용
	CStructuredBuffer* m_RayCollisionOut;
	vector<tRayCollision> m_vecRayColInst;

	// AABB
	array<Vec3, 2> m_AABB;

	// LandScape 모드
	LANDSCAPE_MODE m_Mode;

public:
	void SetFace(UINT PX, UINT PZ);
	void SetMode(LANDSCAPE_MODE PMode) { m_Mode = PMode; }
	void SetBrushIdx(UINT PIdx) { m_BrushIdx = PIdx; }
	void SetWeightIdx(int PIdx) { m_WeightIdx = PIdx; }
	void SetBrushScale(Vec2 PScale) { m_BrushScale = PScale; }

	void SetHeightMapTexture(Ptr<CTexture> PHeightMap) { m_HeightMap = PHeightMap; }
	void CreateHeightMap(UINT PWidth, UINT PHeight);
	void AddBrushTexture(Ptr<CTexture> PBrushTexture) { m_vecBrush.push_back(PBrushTexture); }
	void SetColorTexture(Ptr<CTexture> PArrTex);
	void SetNormalTexture(Ptr<CTexture> PArrTex);

	Vec2 GetFace() const { return {static_cast<float>(m_FaceX), static_cast<float>(m_FaceZ)}; }
	Vec2 GetBrushScale() const { return m_BrushScale; }
	UINT GetBrushIdx() const { return m_BrushIdx; }
	UINT GetWeigtIdx() const { return m_WeightIdx; }
	int GetBrushSize() const { return static_cast<int>(m_vecBrush.size()); }
	int GetWeightSize() const { return m_ColorTex->GetArraySize(); }

	// AABB
	void SetAABB();
	array<Vec3, 2> GetAABB() const { return m_AABB; }

	LANDSCAPE_MODE GetMode() const { return m_Mode; }

	Vec3 GetWorldPosByLandScape(Vec3 PTargetWorldPos) const;

	Vec3 GetWorldPosLandNormal(Vec3& PTargetWorldPos) const;

	// 들어있는 Ray연산(구형 방식)
	tRaycastOut ColliderRaycasting(tRay PRay) const;
	// 들어있는 Ray연산
	void AddRayCol(const tRayCollision& PRayInfo) { m_vecRayColInst.push_back(PRayInfo); }
	void ColisionRayStack(void* PRayObject, const tRay& PRayPosDir);
	vector<tRayCollision>& Collidercalcul();

	void FinalTick() override;
	void Render() override;
	void SaveComponent(FILE* PFile) override;
	void LoadComponent(FILE* PFile) override;

	template <typename T>
	static void BeginOverlap(T* POther);
	template <typename T>
	static void Overlap(T* POther);
	template <typename T>
	static void EndOverlap(T* POther);

private:
	void Init() override;
	void CreateMesh();
	void CreateMaterial();
	void CreateComputeShader();

	int Raycasting();

public:
	CLONE(CLandScape);
	CLandScape();
	~CLandScape() override;
};

template <typename T>
void CLandScape::BeginOverlap(T* POther)
{
	// Do Nothing
}

template <typename T>
void CLandScape::Overlap(T* POther)
{
	// Do Nothing
}

template <typename T>
void CLandScape::EndOverlap(T* POther)
{
	// Do Nothing
}
