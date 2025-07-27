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

class FLandScape :
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

	// LandScape 모드
	LANDSCAPE_MODE m_Mode;

private:
	void Init() override;
	void CreateMesh();
	void CreateMaterial();
	void CreateComputeShader();
	int Raycasting();

public:
	void FinalTick() override;
	void Render() override;
	void SaveComponent(FILE* InFile) override;
	void LoadComponent(FILE* InFile) override;

	// 들어있는 Ray 연산 (구형 방식)
	tRaycastOut ColliderRaycasting(tRay InRay) const;
	// 들어있는 Ray 연산
	void AddRayCol(const tRayCollision& InRayInfo) { m_vecRayColInst.push_back(InRayInfo); }
	void ColisionRayStack(void* InRayObject, const tRay& InRayPosDir);
	vector<tRayCollision>& Collidercalcul();

	// Getter & Setter
	Vec2 GetFace() const { return {static_cast<float>(m_FaceX), static_cast<float>(m_FaceZ)}; }
	Vec2 GetBrushScale() const { return m_BrushScale; }
	UINT GetBrushIdx() const { return m_BrushIdx; }
	UINT GetWeigtIdx() const { return m_WeightIdx; }
	int GetBrushSize() const { return static_cast<int>(m_vecBrush.size()); }
	int GetWeightSize() const { return m_ColorTex->GetArraySize(); }
	LANDSCAPE_MODE GetMode() const { return m_Mode; }

	void SetFace(UINT InX, UINT InZ);
	void SetMode(LANDSCAPE_MODE InMode) { m_Mode = InMode; }
	void SetBrushIdx(UINT InIdx) { m_BrushIdx = InIdx; }
	void SetWeightIdx(int InIdx) { m_WeightIdx = InIdx; }
	void SetBrushScale(Vec2 InScale) { m_BrushScale = InScale; }
	void SetHeightMapTexture(Ptr<CTexture> InHeightMap) { m_HeightMap = InHeightMap; }
	void AddBrushTexture(Ptr<CTexture> InBrushTexture) { m_vecBrush.push_back(InBrushTexture); }

	void CreateHeightMap(UINT InWidth, UINT InHeight);
	void SetColorTexture(Ptr<CTexture> InArrTex);
	void SetNormalTexture(Ptr<CTexture> InArrTex);

	Vec3 GetWorldPosByLandScape(Vec3 PTargetWorldPos) const;
	Vec3 GetWorldPosLandNormal(Vec3& PTargetWorldPos) const;

	// Special Member Function
	FLandScape();
	~FLandScape() override;
	CLONE(FLandScape);
};
