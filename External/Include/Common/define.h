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

#pragma warning(disable: 4005) // DELETE 재정의 경고 배제
#define DELETE(p) if(nullptr != p) {delete p; p = nullptr;}
#define DELETE_ARR(p) if(nullptr != p) {delete[] p; p = nullptr;}

#define MAX_LAYER 32

#define KEY_CHECK(KEY, STATE) CKeyMgr::GetInst()->GetKeyState(KEY) == STATE
#define KEY_TAP(KEY) KEY_CHECK(KEY, KEY_STATE::TAP)
#define KEY_RELEASED(KEY) KEY_CHECK(KEY, KEY_STATE::RELEASED)
#define KEY_PRESSED(KEY) KEY_CHECK(KEY, KEY_STATE::PRESSED)
#define KEY_NONE(KEY) KEY_CHECK(KEY, KEY_STATE::NONE)

#define DT CTimeMgr::GetInst()->GetDeltaTime()
#define EngineDT CTimeMgr::GetInst()->GetEngineDeltaTime()

#define CLONE(Type) virtual Type* Clone() { return new Type(*this); }

#define CLONE_DISABLE(Type) virtual Type* Clone() { return nullptr; }\
							Type(const Type& _Origin) = delete;


#define FONT_RGBA(r, g, b, a) (((((BYTE)a << 24 ) | (BYTE)b << 16) | (BYTE)g << 8) | (BYTE)r)

#define TRIGGER					0x01		// 트리거용 충돌체
#define INDEPENDENT_ROT			0x02

#define SET_PARENT_SCRIPT(PARENT_ENUM) static constexpr SCRIPT_TYPE MParentType = PARENT_ENUM; \
									   virtual SCRIPT_TYPE GetParentScriptType() const override { return MParentType; }

/** Log Macros **/
#define LOG_TRACE(PMessage) LogManager::GetInst()->LogTrace(PMessage)
#define LOG_TRACE_F(PFormat, ...) LogManager::GetInst()->LogTracef(PFormat, ##__VA_ARGS__)
#ifdef _DEBUG
// TODO(KHJ): Debug Mode와 Release Mode Log 분리 작업
#define LOG_DEBUG(PMessage) LogManager::GetInst()->LogDebug(PMessage)
#define LOG_INFO(PMessage) LogManager::GetInst()->LogInfo(PMessage)
#define LOG_WARNING(PMessage) LogManager::GetInst()->LogWarning(PMessage)
#define LOG_ERROR(PMessage) LogManager::GetInst()->LogError(PMessage)
#define LOG_CRITICAL(PMessage) LogManager::GetInst()->LogCritical(PMessage)
#define LOG_UNKNOWN(PMessage) LogManager::GetInst()->LogUnknown(PMessage)

#define LOG_DEBUG_F(PFormat, ...) LogManager::GetInst()->LogDebugf(PFormat, ##__VA_ARGS__)
#define LOG_INFO_F(PFormat, ...) LogManager::GetInst()->LogInfof(PFormat, ##__VA_ARGS__)
#define LOG_WARNING_F(PFormat, ...) LogManager::GetInst()->LogWarningf(PFormat, ##__VA_ARGS__)
#define LOG_ERROR_F(PFormat, ...) LogManager::GetInst()->LogErrorf(PFormat, ##__VA_ARGS__)
#define LOG_CRITICAL_F(PFormat, ...) LogManager::GetInst()->LogCriticalf(PFormat, ##__VA_ARGS__)
#define LOG_UNKNOWN_F(PFormat, ...) LogManager::GetInst()->LogUnknownf(PFormat, ##__VA_ARGS__)
#else
#define LOG_DEBUG(PMessage)
#define LOG_INFO(PMessage)
#define LOG_WARNING(PMessage)
#define LOG_ERROR(PMessage)
#define LOG_CRITICAL(PMessage)
#define LOG_UNKNOWN(PMessage)

#define LOG_DEBUG_F(PFormat, ...)
#define LOG_INFO_F(PFormat, ...)
#define LOG_WARNING_F(PFormat, ...)
#define LOG_ERROR_F(PFormat, ...)
#define LOG_CRITICAL_F(PFormat, ...)
#define LOG_UNKNOWN_F(PFormat, ...)
#endif
