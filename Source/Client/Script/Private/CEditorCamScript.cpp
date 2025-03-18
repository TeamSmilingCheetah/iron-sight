#include "pch.h"
#include "Client/Script/Public/CEditorCamScript.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

CEditorCamScript::CEditorCamScript()
	: CScript(0)
	  , m_CameraSpeed(500.f)
{
}

CEditorCamScript::~CEditorCamScript()
{
}

void CEditorCamScript::Tick()
{
	if (ORTHOGRAPHIC == Camera()->GetProjType())
		CameraOrthgraphicMove();
	else
		CameraPerspectiveMove();
}

void CEditorCamScript::CameraOrthgraphicMove()
{
	Vec3 vWorldPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vWorldPos.y += EngineDT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vWorldPos.y -= EngineDT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vWorldPos.x -= EngineDT * m_CameraSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vWorldPos.x += EngineDT * m_CameraSpeed;
	}

	Transform()->SetRelativePos(vWorldPos);


	float Scale = Camera()->GetScale();

	if (KEY_PRESSED(KEY::NUMPAD_0))
	{
		Scale += EngineDT;
	}

	if (KEY_PRESSED(KEY::NUMPAD_1))
	{
		Scale -= EngineDT;
		if (Scale < 0.1f)
			Scale = 0.1f;
	}

	Camera()->SetScale(Scale);
}

void CEditorCamScript::CameraPerspectiveMove()
{
	float Speed = m_CameraSpeed;
	if (KEY_PRESSED(KEY::LSHIFT))
		Speed *= 5.f;

	Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::FRONT);
	Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * EngineDT * Speed;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * EngineDT * Speed;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * EngineDT * Speed;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * EngineDT * Speed;

	Transform()->SetRelativePos(vPos);

	// 마우스 방향에 따른 오브젝트 회전
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		Vec2 vDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += vDir.x * EngineDT * 15.f;
		vRot.x += vDir.y * EngineDT * 10.f;

		Transform()->SetRelativeRotation(vRot);
	}
}
