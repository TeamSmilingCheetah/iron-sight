#include "pch.h"
#include "func.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "System/Public/Manager/CTaskMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"

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
