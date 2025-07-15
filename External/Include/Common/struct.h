#pragma once

#include <thread>

using std::thread;

struct Vtx
{
	Vec3 vPos;
	Vec2 vUV;
	Vec4 vColor;

	// 표면 방향 벡터
	Vec3 vTangent; // 접선 베터
	Vec3 vNormal; // 법선 벡터
	Vec3 vBinormal; // 종법선 벡터

	// Skinning 용
	Vec4 vWeights; // Bone 가중치
	Vec4 vIndices; // Bone 인덱스
};


struct tDebugShapeInfo
{
	DEBUG_SHAPE Shape;

	Vec3 WorldPos;
	Vec3 Scale;
	Vec3 Rotation;
	Matrix matWorld;

	Vec4 Color;
	float Duration; // DebugShape 유지 시간
	float Time; // 현재 진행 시간

	bool DepthTest; // 깊이검사를 수행

	// general
	DWORD_PTR Data1;
	DWORD_PTR Data2;
};

struct tTask
{
	TASK_TYPE Type;
	DWORD_PTR Param0;
	DWORD_PTR Param1;
	DWORD_PTR Param2;
};

struct tLight2DInfo
{
	Vec3 vColor; // 광원 색상
	Vec3 vWorldPos; // 광원 위치
	Vec3 vDir; // 광원 진행 방향
	int Type; // 광원 타입(LIGHT_TYPE)
	float Radius; // 광원 범위반경
	float Angle; // 광원 범위각도
};


struct tLightColor
{
	Vec3 vColor; // 광원의 빛 자체 색상
	float SpecCoeff; // 반사 계수
	Vec3 vAmbient; // 환경광(광원으로 인해서 생긴 최소한의 빛)
};

struct tLight3DInfo
{
	tLightColor info; // 광원 색상 정보
	LIGHT_TYPE Type; // 광원 타입(LIGHT_TYPE)
	Vec3 WorldPos; // 광원 위치
	float Radius; // 광원 범위반경 (Point)
	Vec3 Dir; // 광원 진행 방향
	float Angle; // 광원 범위각도	(Spot)
};

// Pixel
struct tPixel
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};


// Particle
struct tParticle
{
	Vec4 vColor;
	Vec3 vLocalPos;
	Vec3 vWorldPos;
	Vec3 vWorldInitScale;
	Vec3 vWorldCurrentScale;
	Vec3 vWorldRotation;

	Vec3 vForce; // Particle 에 누적된 힘
	Vec3 vVelocity;

	float NoiseForceAccTime; // Noise Force 텀에 도달하는 누적 시간
	Vec3 NoiseForceDir; // Noise Force 의 방향

	float Mass;
	float Life;
	float Age;
	float NormalizedAge;
	int Active;

	Vec2 Padding;
};

// Particle Module
struct tParticleModule
{
	// Spawn
	UINT SpawnRate; // 초당 파티클 생성개수 (Spawn Per Second)
	Vec4 vSpawnColor; // 생성 시점 색상
	Vec4 vSpawnMinScale; // 생성 시 최소 크기
	Vec4 vSpawnMaxScale; // 생성 시 최대 크기

	float MinLife; // 최소 수명
	float MaxLife; // 최대 수명

	UINT SpawnShape; // 0 : Box,  1: Sphere
	Vec3 SpawnShapeScale; // SpawnShapeScale.x == Radius

	UINT BlockSpawnShape; // 0 : Box,  1: Sphere
	Vec3 BlockSpawnShapeScale; // SpawnShapeScale.x == Radius

	UINT SpaceType; // 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT SpawnBurstCount; // 한번에 발생시키는 Particle 수
	UINT SpawnBurstRepeat;
	float SpawnBurstRepeatTime;

	// Add Velocity
	UINT AddVelocityType; // 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed
	Vec3 AddVelocityFixedDir;
	float AddMinSpeed;
	float AddMaxSpeed;

	// Scale Module
	float StartScale;
	float EndScale;

	// Drag Module (감속 모듈)
	float DestNormalizedAge;
	float LimitSpeed;

	// Noise Force Module
	float NoiseForceTerm; // Noise Force 적용시키는 텀
	float NoiseForceScale; // Noise Force 크기

	// Render Module
	Vec3 EndColor; // 최종 색상
	UINT FadeOut; // 0 : Off, 1 : Normalized Age
	float FadeOutStartRatio; // FadeOut 효과가 시작되는 Normalized Age 지점
	UINT VelocityAlignment; // 속도 정렬 0 : Off, 1 : On


	// Module On / Off
	int Module[(UINT)PARTICLE_MODULE::END];
};


struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};


// UI
struct PayLoad
{
	wstring Type;
	DWORD_PTR Data;
};

// Font
struct FontRenderInfo
{
	// 폰트 정보
	wstring Font;
	UINT FontSize;

	// 출력 정보
	wstring Text;
	UINT Color;
	Vec2 Pos;
	Vec2 Layout; // width, height (텍스트가 끊길 위치)
	Vec4 Clip; // left, top, right, bottom
};

// Bullet
struct BULLETINFO
{
	CGameObject* m_Owner; // 총알 주인
	CGameObject* m_ShotWeapon; // 해당 총알을 발사한 무기
	float m_Dmg; // 총알 데미지
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4 vTranslate;
	Vec4 vScale;
	Vec4 qRot;
};

struct tMTKeyFrame
{
	double dTime;
	int iFrame;
	Vec3 vTranslate;
	Vec3 vScale;
	Vec4 qRot;
};


struct tMTBone
{
	wstring strBoneName;
	int iDepth;
	int iParentIndx;
	Matrix matOffset; // Inverse 행렬( Skin 정점을 -> 기본상태로 되돌림)
	Matrix matBone;
};

// ===========
// Instancing
// ===========
union uInstID
{
	struct
	{
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};

	ULONG64 llID;
};

class CGameObject;

struct tInstObj
{
	CGameObject* pObj;
	UINT iMtrlIdx;
};

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int iRowIdx;
	int parentID;
	int objectID;
	int reserved[2]; // 패딩
};


// ====================
// 상수 버퍼 연동 구조체
// ====================
struct TransInfo
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;
	Matrix matWVP;
};

extern TransInfo g_Trans;

// Material 계수
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};

struct MtrlConst
{
	tMtrlData mtrl;
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];
	int bTex[TEX_PARAM::TEX_END];

	// 3D Animation 정보
	int arrAnimData[4];
};

struct SpriteInfo
{
	Vec2 LeftTopUV;
	Vec2 SliceUV;
	Vec2 BackgroundUV;
	Vec2 OffsetUV;
	int SpriteUse;
	int padding[3];
};

struct GlobalData
{
	Vec2 RenderResolution; // 렌더링 해상도

	float DeltaTime; // DT
	float Time; // 누적시간

	float DT_Engine; // DT
	float Time_Engine; // 누적시간

	int Light2DCount; // 2D 광원 개수
	int Light3DCount; // 3D 광원 개수
};

// Graphic Shader

struct tScalarParam
{
	string Desc;
	SCALAR_PARAM eParam;
	bool Drag;
};

struct tTexParam
{
	string Desc;
	TEX_PARAM eParam;
};

extern GlobalData g_Data;

// Mesh Collision

struct MeshCollisionInfo
{
	UINT LeftTriCount;
	UINT RightTriCount;
	UINT Padding[2];
};

struct CollisionResult
{
	Vec3 LeftNormal;
	Vec3 RightNormal;
	float PenetrationDepth;
};

// Facade Struct

struct UnsignedIntegerSquare
{
	UINT X;
	UINT Y;
};

// Log Struct

struct LogMessage
{
	ELogLevel LogLevel;
	string Message;
	string TimeStamp;
	thread::id ThreadID;

	LogMessage(ELogLevel PLevel, const string& PMessage);
};

struct AABB
{
	Vec3 Min;
	Vec3 Max;

	AABB(Vec3 PMin, Vec3 PMax)
	{
		Min = PMin;
		Max = PMax;
	}

	AABB()
	{
		Min = Vec3(FLT_MAX);
		Max = Vec3(FLT_MIN);
	}

	void Expand(const AABB& POther);

	Vec3 Center() const;
	int LongestAxis() const;
	bool Intersects(const AABB& POther) const;
};

struct BVHNode
{
	AABB Bounds;
	vector<CGameObject*> Objects;
	BVHNode* Left = nullptr;
	BVHNode* Right = nullptr;

	~BVHNode()
	{
		// Cascade Delete
		delete Left;
		delete Right;
	}

	bool IsLeaf() const;
};
