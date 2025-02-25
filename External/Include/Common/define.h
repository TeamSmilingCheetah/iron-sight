#pragma once


using Vec2 = SimpleMath::Vector2;
using Vec3 = SimpleMath::Vector3;
using Vec4 = SimpleMath::Vector4;
using Matrix = SimpleMath::Matrix;
using Quaternion = SimpleMath::Quaternion;

using GAMEOBJECT_SAVE = int(*)(class CGameObject*, FILE*);
using GAMEOBJECT_LOAD = class CGameObject* (*)(FILE*);


#define RELEASE(Inst) if(nullptr != Inst) Inst->Release();

#define DEVICE CDevice::GetInst()->GetDevice().Get()
#define CONTEXT CDevice::GetInst()->GetContext().Get()

#define DELETE(p) if(nullptr != p) {delete p; p = nullptr;}
#define DELETE_ARR(p) if(nullptr != p) {delete[] p; p = nullptr;}

#define MAX_LAYER 32

#define KEY_CHECK(KEY, STATE) CKeyMgr::GetInst()->GetKeyState(KEY) == STATE
#define KEY_TAP(KEY) KEY_CHECK(KEY, KEY_STATE::TAP)
#define KEY_RELEASED(KEY) KEY_CHECK(KEY, KEY_STATE::RELEASED)
#define KEY_PRESSED(KEY) KEY_CHECK(KEY, KEY_STATE::PRESSED)

#define DT CTimeMgr::GetInst()->GetDeltaTime()
#define EngineDT CTimeMgr::GetInst()->GetEngineDeltaTime()

#define CLONE(Type) virtual Type* Clone() { return new Type(*this); }

#define CLONE_DISABLE(Type) virtual Type* Clone() { return nullptr; }\
							Type(const Type& _Origin) = delete;
