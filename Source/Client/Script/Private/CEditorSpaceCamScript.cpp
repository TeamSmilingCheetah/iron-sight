#include "pch.h"
#include "Client/Script/Public/CEditorSpaceCamScript.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

CEditorSpaceCamScript::CEditorSpaceCamScript()
	: CScript(SCRIPT_TYPE::NONE)
	, m_CameraSpeed(500.f)
{
}

CEditorSpaceCamScript::~CEditorSpaceCamScript()
{
}

void CEditorSpaceCamScript::Tick()
{
	CameraPerspectiveMove();
}

void CEditorSpaceCamScript::CameraPerspectiveMove()
{
	assert(PERSPECTIVE == Camera()->GetProjType());

	if (KEY_TAP(KEY::RBTN) || KEY_TAP(KEY::LBTN))
	{
		m_OriginMousePos = CKeyMgr::GetInst()->GetMousePos();
	}

	else if (KEY_PRESSED(KEY::RBTN))
	{
		// x축을 rotate함 (방향벡터이므로 scale, translate는 적용 어차피 x)
		Vec3 xAxis = Vec3(1.f, 0.f, 0.f);
		xAxis = Vec3::TransformNormal(xAxis, Transform()->GetWorldMat());
		xAxis.Normalize();

		Vec3 yAxis = Vec3(0.f, 1.f, 0.f);
		yAxis = Vec3::TransformNormal(yAxis, Transform()->GetWorldMat());
		yAxis.Normalize();

		Vec2 CurMousePos = CKeyMgr::GetInst()->GetMousePos();
		Vec2 DeltaMousePos = CurMousePos - m_OriginMousePos;

		Vec3 vPos = Transform()->GetRelativePos();
		vPos -= DeltaMousePos.x * xAxis;
		vPos += DeltaMousePos.y * yAxis;

		Transform()->SetRelativePos(vPos);

		m_OriginMousePos = CurMousePos;
	}

	else if (KEY_PRESSED(KEY::LBTN))
	{
		Vec3 xAxis = Vec3(1.f, 0.f, 0.f);
		xAxis = Vec3::TransformNormal(xAxis, Transform()->GetWorldMat());
		xAxis.Normalize();

		Vec3 yAxis = Vec3(0.f, 1.f, 0.f);
		//yAxis = Vec3::TransformNormal(yAxis, Transform()->GetWorldMat());
		//yAxis.Normalize();

		Vec2 CurMousePos = CKeyMgr::GetInst()->GetMousePos();
		Vec2 DeltaMousePos = CurMousePos - m_OriginMousePos;

		// 1. 방향 회전
		Transform()->RotateAxis(xAxis, DeltaMousePos.y * 0.3f);
		Transform()->RotateAxis(yAxis, DeltaMousePos.x * 0.3f);

		// 2. 위치 설정
		Vec3 vPos = Transform()->GetRelativePos();

		float xAngle = XMConvertToRadians(DeltaMousePos.y * 0.3f) / 2.f;
		float yAngle = XMConvertToRadians(DeltaMousePos.x * 0.3f) / 2.f;

		Quaternion xRot = Quaternion(xAxis * sinf(xAngle), cosf(xAngle));
		Quaternion yRot = Quaternion(yAxis * sinf(yAngle), cosf(yAngle));

		vPos = Vec3::Transform(vPos, xRot * yRot);

		Transform()->SetRelativePos(vPos);

		m_OriginMousePos = CurMousePos;
	}

	//if (KEY_PRESSED(KEY::WHEEL))
	
}
