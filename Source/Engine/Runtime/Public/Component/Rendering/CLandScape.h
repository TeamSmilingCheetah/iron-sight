#pragma once
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"
#include "Engine/System/Public/Rendering/Shader/CHeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CWeightMapCS.h"
#include "Engine/System/Public/Rendering/Shader/CRaycastCS.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

struct tRaycastOut
{
    Vec2 Location;
    UINT Distance;
    int Success;
};

struct tRayCollision
{
	void*	RayObj;
	Vec3	RayWorldPos;
	Vec3	RayPos;
	Vec3	RayDir;
	float	RayLength;
	UINT	Distance;
	int		Success;
	int		Padding[2];

	tRayCollision()
		: RayObj(nullptr)
		, RayLength(0)
		, Distance(0xffffffff)
		, Success(0)
		, Padding{0}
	{

	}

	tRayCollision(void* _ID, tRay _Ray)
		: RayObj(_ID)
		, RayLength(0)
		, RayPos(_Ray.vStart)
		, RayDir(_Ray.vDir)
		, Distance(0xffffffff)
		, Success(0)
		, Padding{0}
	{
	}
};

enum LANDSCAPE_MODE
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
	vector<float> m_CachedHeightData;	// 높이맵 데이터

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
	CStructuredBuffer*		m_RayCollisionOut;
	vector<tRayCollision>	m_vecRayColInst;

    // LandScape 모드
    LANDSCAPE_MODE m_Mode;

public:
    void SetFace(UINT _X, UINT _Z);
	void SetMode(LANDSCAPE_MODE _Mode) { m_Mode = _Mode; }
	void SetBrushIdx(UINT _Idx) { m_BrushIdx = _Idx; }
	void SetWeightIdx(int _Idx) { m_WeightIdx = _Idx; }
	void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

	void SetHeightMapTexture(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void CreateHeightMap(UINT _Width, UINT _Height);
    void AddBrushTexture(Ptr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }
    void SetColorTexture(Ptr<CTexture> _ArrTex);
    void SetNormalTexture(Ptr<CTexture> _ArrTex);

	Vec2 GetFace() { return Vec2(static_cast<float>(m_FaceX), static_cast<float>(m_FaceZ));	}
	Vec2 GetBrushScale() { return m_BrushScale; }
	UINT GetBrushIdx() { return m_BrushIdx; }
	UINT GetWeigtIdx() { return m_WeightIdx; }
	int GetBrushSize() { return static_cast<int>(m_vecBrush.size()); }
	int GetWeightSize() { return m_ColorTex->GetArraySize(); }


	LANDSCAPE_MODE GetMode() { return m_Mode; }

	Vec3 GetWorldPosByLandScape(Vec3 _TargetWorldPos);

	Vec3 GetWorldPosLandNormal(Vec3& _TargetWorldPos);

	// 들어있는 Ray연산(구형 방식)
	tRaycastOut ColliderRaycasting(tRay _Ray);
	// 들어있는 Ray연산
	void AddRayCol(const tRayCollision& _RayInfo) { m_vecRayColInst.push_back(_RayInfo); }
	void ColisionRayStack(void* _RayObj, tRay _RayPosDir);
	vector<tRayCollision>& Collidercalcul();

    void FinalTick() override;
    void Render() override;
    void SaveComponent(FILE* _File) override;
    void LoadComponent(FILE* _File) override;

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
