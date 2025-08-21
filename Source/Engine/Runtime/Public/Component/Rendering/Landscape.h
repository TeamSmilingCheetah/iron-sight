#pragma once

#include <array>

#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Rendering/Shader/CHeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CWeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"
#include "Engine/Runtime/Public/Component/Base/RenderComponent.h"

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

	tRayCollision(void* InID, const Vec3& InRayPos, const Vec3& InRayDir)
		: RayObj(InID)
		  , RayPos(InRayPos)
		  , RayDir(InRayDir)
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

/**
 * @brief 게임 엔진에서 딛고 서는 지형지물을 담당하는 클래스
 *
 * @param m_FaceX 지형 가로 면 개수
 * @param m_FaceZ 지형 세로 면 개수
 * @param m_HeightMap 지형에 적용시킬 높이맵
 * @param m_CachedHeightData 높이맵 데이터
 */
class FLandscape :
	public FRenderComponent
{
private:
	UINT FaceX;
	UINT FaceZ;

	Ptr<CTexture> HeightMap;
	vector<float> CachedHeightData;

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

	Vec3 GetWorldPosByLandScape(const Vec3& InTargetWorldPosition) const;
	Vec3 GetWorldPosLandNormal(Vec3& InTargetWorldPosition) const;

	void SetFace(UINT InX, UINT InZ);
	void CreateHeightMap(UINT PWidth, UINT PHeight);

	vector<tRayCollision>& Collidercalcul();

	// Getter & Setter
	Vec2 GetFace() const { return {static_cast<float>(FaceX), static_cast<float>(FaceZ)}; }
	void SetHeightMapTexture(Ptr<CTexture> InHeightMap) { HeightMap = InHeightMap; }

	// Special Member Function
	FLandscape();
	~FLandscape() override;
	CLONE(FLandscape);
};

// [[deprecated]]
// LANDSCAPE_MODE m_Mode; // LandScape 모드
// vector<Ptr<CTexture>> m_vecBrush;
// Vec2 m_BrushScale; // 브러쉬 크기
// UINT m_BrushIdx; // 브러쉬 인덱스
// Ptr<CHeightMapCS> m_HeightMapCS; // 높이맵 수정용 컴퓨트 쉐이더
// WeightMap
// Ptr<CTexture> m_ColorTex;
// Ptr<CTexture> m_NormalTex;
// CStructuredBuffer* m_WeightMap;
// UINT m_WeightWidth;
// UINT m_WeightHeight;
// Ptr<CWeightMapCS> m_WeightMapCS;
// int m_WeightIdx;
// Collision연산용
// CStructuredBuffer* m_RayCollisionOut;
// vector<tRayCollision> m_vecRayColInst;
// Raycasting
// Ptr<CRaycastCS> m_RaycastCS;
// CStructuredBuffer* m_RaycastOut; // 마우스 픽킹 위치
// tRaycastOut m_Out; // 마우스 픽킹 위치
// vector<tRaycastOut> m_OutCol; // Ray충돌체용
// void AddBrushTexture(Ptr<CTexture> PBrushTex) { m_vecBrush.push_back(PBrushTex); }
// 들어있는 Ray 연산 (Old Type)
// tRaycastOut ColliderRaycasting(tRay PRay);
// 들어있는 Ray 연산
// void AddRayCol(const tRayCollision& PRayInfo) { m_vecRayColInst.push_back(PRayInfo); }
// void ColisionRayStack(void* PRayObj, tRay* PRayPosDir);
// void SetColorTexture(Ptr<CTexture> PArrTex);
// void SetNormalTexture(Ptr<CTexture> PArrTex);
