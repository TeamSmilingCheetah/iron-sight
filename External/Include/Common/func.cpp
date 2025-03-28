#include "pch.h"
#include "func.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "System/Public/Manager/CLevelMgr.h"

void SaveObjectRef(CGameObject* _Object, FILE* _File)
{
	UINT ObjectID = 0xFFFFFFFF;

	if (_Object)
		ObjectID = _Object->GetObjectID();

	fwrite(&ObjectID, sizeof(UINT), 1, _File);
}

void LoadObjectRef(CGameObject*& _MissingAddress, FILE* _File)
{
	UINT ObjectID = 0;
	fread(&ObjectID, sizeof(UINT), 1, _File);

	// Object 참조가 없는 경우
	if (ObjectID == 0xFFFFFFFF)
		return;

	CLevelMgr::GetInst()->AddObjectRefResolution(_MissingAddress, ObjectID);
}

void CreateObject(CGameObject* _NewObj, int LayerIdx, bool _bMoveChild)
{
	tTask task = {};
	task.Type = TASK_TYPE::CREATE_OBJECT;
	task.Param0 = (DWORD_PTR)_NewObj;
	task.Param1 = LayerIdx;
	task.Param2 = _bMoveChild;

	CTaskMgr::GetInst()->AddTask(task);
}

void DestroyObject(CGameObject* _TargetObj)
{
	tTask task = {};
	task.Type = TASK_TYPE::DELETE_OBJECT;
	task.Param0 = (DWORD_PTR)_TargetObj;

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeLevelState(LEVEL_STATE _NextState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
	task.Param0 = static_cast<DWORD_PTR>(_NextState);

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeLevel(CLevel* _Level, LEVEL_STATE _NextState)
{
	tTask task = {};

	task.Type = TASK_TYPE::CHANGE_LEVEL;
	task.Param0 = (DWORD_PTR)_Level;
	task.Param1 = static_cast<DWORD_PTR>(_NextState);

	CTaskMgr::GetInst()->AddTask(task);
}

void AddChild(CGameObject* _Parent, CGameObject* _Child)
{
	tTask task = {};

	task.Type = TASK_TYPE::ADD_CHILD;
	task.Param0 = (DWORD_PTR)_Parent;
	task.Param1 = (DWORD_PTR)_Child;

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeName(CEntity* _Entity, wstring* _Name)
{
	tTask task{};
	task.Type = TASK_TYPE::CHANGE_NAME;
	task.Param0 = (DWORD_PTR)_Entity;
	task.Param1 = (DWORD_PTR)_Name;

	CTaskMgr::GetInst()->AddTask(task);
}

bool IsValid(CGameObject*& _Object)
{
	if (nullptr == _Object || _Object->IsDead())
	{
		_Object = nullptr;
		return false;
	}

	return true;
}

void DrawDebugRect(Vec4 _Color, Vec3 _Pos, Vec2 _Scale, Vec3 _Rotation, bool _DepthTest,
				   float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;

	info.WorldPos = _Pos;
	info.Scale = Vec3(_Scale.x, _Scale.y, 1.f);
	info.Rotation = _Rotation;
	info.matWorld = XMMatrixIdentity();

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugRect(Vec4 _Color, const Matrix& _matWorld, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;

	info.matWorld = _matWorld;

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugCircle(Vec4 _Color, Vec3 _Pos, float _Radius, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;
	info.Color = _Color;
	info.WorldPos = _Pos;
	info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, 1.f);
	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugCube(Vec4 _Color, Vec3 _Pos, Vec3 _Scale, Vec3 _Rotation, bool _DepthTest,
				   float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CUBE;
	info.Color = _Color;

	info.WorldPos = _Pos;
	info.Scale = _Scale;
	info.Rotation = _Rotation;
	info.matWorld = XMMatrixIdentity();

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugCube(Vec4 _Color, const Matrix& _matWorld, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CUBE;
	info.Color = _Color;

	info.matWorld = _matWorld;

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugSphere(Vec4 _Color, Vec3 _Pos, float _Radius, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::SPHERE;
	info.Color = _Color;
	info.WorldPos = _Pos;
	info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, _Radius * 2.f);
	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugLine(Vec4 _Color, Vec3 _Start, Vec3 _vEnd, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::LINE;
	info.Color = _Color;
	info.WorldPos = _Start;
	info.Scale = _vEnd;
	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugSkeleton(Vec4 _Color, const Matrix& _matWorld, CStructuredBuffer* _PureBoneMat, CStructuredBuffer* _ParentIdx, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::SKELETON;
	info.Color = _Color;
	info.Data1 = reinterpret_cast<DWORD_PTR>(_PureBoneMat);
	info.Data2 = reinterpret_cast<DWORD_PTR>(_ParentIdx);
	info.matWorld = _matWorld;
	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

Matrix GetMatrixFromFbxMatrix(const FbxAMatrix& _mat)
{
	Matrix mat{};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = static_cast<float>(_mat.Get(i, j));
		}
	}
	return mat;
}

Vec4 GetVectorFromFbxVector(const FbxDouble4& _Vec)
{
	Vec4 vec{};
	for (int i = 0; i < 4; ++i)
	{
		vec[i] = static_cast<float>(_Vec.mData[i]);
	}
	return vec;
}

void SaveWString(const wstring& _str, FILE* _File)
{
	size_t Len = _str.length();
	fwrite(&Len, sizeof(size_t), 1, _File);
	fwrite(_str.c_str(), sizeof(wchar_t), Len, _File);
}

void LoadWString(wstring& _str, FILE* _File)
{
	wchar_t szBuffer[255] = {};

	size_t Len = 0;
	fread(&Len, sizeof(size_t), 1, _File);
	fread(szBuffer, sizeof(wchar_t), Len, _File);
	_str = szBuffer;
}

string WStringToString(const wstring& _str)
{
	// WideCharToMultiByte를 사용한 안전한 변환
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, _str.c_str(), static_cast<int>(_str.size()), nullptr, 0, nullptr, nullptr);
	string res_str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, _str.c_str(), static_cast<int>(_str.size()), &res_str[0], size_needed, nullptr, nullptr);

	return res_str;
}


void GetComponentsNames(vector<wstring>& _vecComponentsNames)
{
	_vecComponentsNames.push_back(L"Transform");
	_vecComponentsNames.push_back(L"Collider2D");
	_vecComponentsNames.push_back(L"Collider3D");
	_vecComponentsNames.push_back(L"ColliderRay");
	_vecComponentsNames.push_back(L"FlipbookPlayer");
	_vecComponentsNames.push_back(L"Camera");
	_vecComponentsNames.push_back(L"Light3D");
	_vecComponentsNames.push_back(L"MeshRender");
	_vecComponentsNames.push_back(L"Decal");
	_vecComponentsNames.push_back(L"LandScape");
	_vecComponentsNames.push_back(L"SkyBox");
	_vecComponentsNames.push_back(L"Tilemap");
	_vecComponentsNames.push_back(L"ParticleSystem");
}
  
bool IntersectsRay(const Vec3* const Pos[3], const Vec3& vStart, const Vec3& vDir, Vec3& pCrossPos, float& pDist)
{
	// 삼각형 표면 방향 벡터
	Vec3 Edge[2] = { Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f) };
	Edge[0] = *Pos[1] - *Pos[0];
	Edge[1] = *Pos[2] - *Pos[0];

	// 삼각형에 수직방향인 법선(Normal) 벡터
	Vec3 Normal = Edge[0].Cross(Edge[1]);
	Normal.Normalize();

	// 삼각형 법선벡터와 Ray 의 Dir 을 내적
	// 광선에서 삼각형으로 향하는 수직벡터와, 광선의 방향벡터 사이의 cos 값
	float NdotD = Normal.Dot(vDir) * -1.f;

	// 내적 값이 0에 가까운지 확인 (평행한 경우)
	const float epsilon = 0.0001f; // 적절한 임의의 상수
	if (abs(NdotD) < epsilon)
		return false;  // 광선이 삼각형 평면과 평행하므로 교차하지 않음

	Vec3 vStoP0 = vStart - *Pos[0];
	float VerticalDist = Normal.Dot(vStoP0); // 광선을 지나는 한점에서 삼각형 평면으로의 수직 길이

	// 광선이 진행하는 방향으로, 삼각형을 포함하는 평면까지의 거리
	float RtoTriDist = VerticalDist / NdotD;

	// 광선이, 삼각형을 포함하는 평면을 지나는 교점
	Vec3 vCrossPoint = vStart + vDir * RtoTriDist;

	// 교점이 삼각형 내부인지 테스트
	Vec3 P0toCross = vCrossPoint - *Pos[0];

	Vec3 Full = Edge[0].Cross(Edge[1]);
	Vec3 U = Edge[0].Cross(P0toCross);
	Vec3 V = P0toCross.Cross(Edge[1]);

	// 직선과 삼각형 평면의 교점이 삼각형 1번과 2번 사이에 존재하는지 체크
	//      0
	//     /  \
	//    1 -- 2    
	if (Full.Dot(U) < 0.f || Full.Dot(V) < 0.f)
		return false;

	// 교점이 삼각형 내부인지 체크
	if (Full.Length() < U.Length() + V.Length())
		return false;

	pCrossPos = vCrossPoint;
	pDist = RtoTriDist;

	return true;
}

float RandomFloat(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 engine(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(engine);
}
