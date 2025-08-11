#include "pch.h"
#include "func.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "System/Public/Manager/CLevelMgr.h"
#include "Runtime/Public/Component/Base/components.h"
#include "Engine/System/Public/Manager/CObjectPoolMgr.h"

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

void ExcutePreload()
{
	Ptr<CPrefab> PoolPrefab = CAssetMgr::GetInst()->Load<CPrefab>(L"Prefab\\9mm.pref", L"Prefab\\9mm.pref");
	CObjectPoolMgr::GetInst()->Preload(PoolPrefab->GetProtoObject()->GetName(), 30);

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
	if (_TargetObj->IsDead())
		return;

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

void ChangeLayer(CGameObject* _TargetObj, LONGLONG _LayerIdx)
{
	if (_TargetObj->GetLayerIdx() == _LayerIdx)
		return;

	tTask task{};
	task.Type = TASK_TYPE::CHANGE_LAYEROBJECT;
	task.Param0 = (DWORD_PTR)_TargetObj;
	task.Param1 = (DWORD_PTR)_LayerIdx;

	CTaskMgr::GetInst()->AddTask(task);
}

void SetObjectActive(CGameObject* _TargetObj, bool _Active)
{
	assert(_TargetObj);

	tTask task = {};
	task.Type = TASK_TYPE::SETACTIVE_OBJECT;
	task.Param0 = (DWORD_PTR)_TargetObj;
	task.Param1 = (DWORD_PTR)_Active;

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

/**
 * @brief AABB 연산이 잘 완료되었는지 값으로 파악하는 함수
 *
 * @param PBox GetAABB 함수 호출로 받은 Vec3 Array
 * @return AABB 연산이 잘 완료되었는지 여부
 */
bool IsNullAABB(const AABB& PBox)
{
	return PBox.Min == Vec3(FLT_MAX) && PBox.Max == Vec3(FLT_MIN);
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

wstring ToLower(const wstring& _str)
{
	wstring result = _str;
	std::transform(result.begin(), result.end(), result.begin(), towlower);
	return result;
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

/**
 * 타입 기반의 스크립트 탐색 함수
 * 우선 정확한 스크립트 탐색을 우선하되 필요에 따라서 부모 스크립트를 리턴한다
 *
 * @param _Object [IN] 스크립트를 보유하고 있는 오브젝트
 * @param _Type [IN] 스크립트의 타입 enum
 * @return 실제 해당 오브젝트가 보유 중인 스크립트
 */
CScript* GetScriptWithType(CGameObject* _Object, SCRIPT_TYPE _Type)
{
	const auto& shortcut = _Object->GetScriptShortcut();
	auto iter = shortcut.find(_Type);

	// not assigned script
	if (iter == shortcut.end())
		return nullptr;

	vector<CScript*> script_vector = _Object->GetScripts();

	// parent type search
	if (script_vector[iter->second] == nullptr)
	{
		auto parent_iter = shortcut.find(script_vector[iter->second]->GetParentScriptType());
		// assure parent type existence
		assert(script_vector[parent_iter->second] != nullptr);
		return script_vector[parent_iter->second];
	}

	return script_vector[iter->second];
}

Vec3 CalcColiisionDir(CGameObject* _TargetObj, CGameObject* _SubObj)
{
	Vec3 vTargetPos = _TargetObj->Transform()->GetRelativePos();
	Vec3 vTargetCollOffset = _TargetObj->BoxCollider()->GetOffset();
	Vec3 vTargetCollScale = _TargetObj->BoxCollider()->GetScale();

	Vec3 vSubPos = _SubObj->Transform()->GetRelativePos();

	Vec3 vTargetScale = _TargetObj->Transform()->GetRelativeScale();
	Vec3 vSubScale = _SubObj->Transform()->GetRelativeScale();

	Vec3 vTargerCenter = Vec3(vTargetPos.x + +vTargetCollOffset.x
							, vTargetPos.y + vTargetCollOffset.y
							, vTargetPos.z + +vTargetCollOffset.z);

	Vec3 vDelta = vTargerCenter - vSubPos;

	// 각 축별 절대값
	float absX = fabs(vDelta.x);
	float absY = fabs(vDelta.y);
	float absZ = fabs(vDelta.z);

	// 축별로 충돌이 얼마나 깊은지
	float overlapX = (vTargetCollScale.x / 2.f + vSubScale.x / 2.f) - absX;
	float overlapY = (vTargetCollScale.y / 2.f + vSubScale.y / 2.f) - absY;
	float overlapZ = (vTargetCollScale.z / 2.f + vSubScale.z / 2.f) - absZ;

	// 최소 오버랩 축 결정
	if (overlapX < overlapY && overlapX < overlapZ)
	{
		// X축 충돌
		return Vec3((vDelta.x > 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0f);
	}
	else if (overlapY < overlapZ)
	{
		// Y축 충돌
		return Vec3(0.0f, (vDelta.y > 0.0f) ? 1.0f : -1.0f, 0.0f);
	}
	else
	{
		// Z축 충돌
		return Vec3(0.0f, 0.0f, (vDelta.z > 0.0f) ? 1.0f : -1.0f);
	}
}

float FloatLerp(float _Cur, float _Des, float _Speed)
{
	float t = 1.f - expf(_Speed * DT);
	return _Cur + (_Des - _Cur) * _Speed * t;
}

float MoveToward(float _Cur, float _Des, float _maxDelta)
{
	float delta = _Des - _Cur;
	if (abs(delta) <= _maxDelta)
		return _Des;
	return _Cur + (delta > 0 ? 1.f : -1.f) * _maxDelta;
}

Vec3 Vec3Lerp(const Vec3& _Cur, const Vec3& _Des, float _Speed)
{
	Vec3 result;
	result.x = _Cur.x + (_Des.x - _Cur.x) * _Speed * DT;
	result.y = _Cur.y + (_Des.y - _Cur.y) * _Speed * DT;
	result.z = _Cur.z + (_Des.z - _Cur.z) * _Speed * DT;
	return result;
}

Vec3 Vec3MoveToward(const Vec3& _Cur, const Vec3& _Des, float _MaxDelta)
{
	Vec3 result;
	float deltaX = _Des.x - _Cur.x;
	result.x = (abs(deltaX) <= _MaxDelta) ? _Des.x : _Cur.x + (deltaX > 0.f ? 1.f : -1.f) * _MaxDelta;
	float deltaY = _Des.y - _Cur.y;
	result.y = (abs(deltaY) <= _MaxDelta) ? _Des.y : _Cur.y + (deltaY > 0.f ? 1.f : -1.f) * _MaxDelta;
	float deltaZ = _Des.z - _Cur.z;
	result.z = (abs(deltaZ) <= _MaxDelta) ? _Des.z : _Cur.z + (deltaZ > 0.f ? 1.f : -1.f) * _MaxDelta;
	return result;
}

/**
 * @brief Variant 그대로 사용할 경우 타입 추론이 어려워지므로, 이를 피하기 위한 ColliderBase 포인터 변환 함수
 * @param InVariant Variant로 선언된 충돌체
 * @return ColliderBase 베이스 클래스 포인터로 정의된 포인터
 */
IColliderBase* GetBaseFromVariant(const ColliderVariant& InVariant)
{
	return visit([](auto* Collider) -> IColliderBase* { return Collider; }, InVariant);
}
