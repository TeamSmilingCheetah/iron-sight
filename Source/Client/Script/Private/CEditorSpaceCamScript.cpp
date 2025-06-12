#include "pch.h"
#include "Client/Script/Public/CEditorSpaceCamScript.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"

#include "imgui/imgui.h"

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

	// 초기화
	m_MouseDelta.x = 0.f;
	m_MouseDelta.y = 0.f;

	m_MouseBtn = ImGuiMouseButton_COUNT;
}

void CEditorSpaceCamScript::CameraPerspectiveMove()
{
	assert(PERSPECTIVE == Camera()->GetProjType());

	switch (m_MouseBtn)
	{
	case ImGuiMouseButton_Left:
	{
		// x축을 rotate함 (방향벡터이므로 scale, translate는 적용 어차피 x)
		Vec3 xAxis = Vec3(1.f, 0.f, 0.f);
		xAxis = Vec3::TransformNormal(xAxis, Transform()->GetWorldMat());
		xAxis.Normalize();

		Vec3 yAxis = Vec3(0.f, 1.f, 0.f);
		yAxis = Vec3::TransformNormal(yAxis, Transform()->GetWorldMat());
		yAxis.Normalize();

		Vec3 vPos = Transform()->GetRelativePos();
		vPos -= m_MouseDelta.x * xAxis;
		vPos += m_MouseDelta.y * yAxis;

		Transform()->SetRelativePos(vPos);
	}
		break;

	case ImGuiMouseButton_Right:
	{
		Vec3 xAxis = Vec3(1.f, 0.f, 0.f);
		xAxis = Vec3::TransformNormal(xAxis, Transform()->GetWorldMat());
		xAxis.Normalize();

		Vec3 yAxis = Vec3(0.f, 1.f, 0.f);
		//yAxis = Vec3::TransformNormal(yAxis, Transform()->GetWorldMat());
		//yAxis.Normalize();

		// 1. 방향 회전
		Transform()->RotateAxis(xAxis, m_MouseDelta.y * 0.3f);
		Transform()->RotateAxis(yAxis, m_MouseDelta.x * 0.3f);

		// 2. 위치 설정
		Vec3 vPos = Transform()->GetRelativePos();

		float xAngle = XMConvertToRadians(m_MouseDelta.y * 0.3f) / 2.f;
		float yAngle = XMConvertToRadians(m_MouseDelta.x * 0.3f) / 2.f;

		Quaternion xRot = Quaternion(xAxis * sinf(xAngle), cosf(xAngle));
		Quaternion yRot = Quaternion(yAxis * sinf(yAngle), cosf(yAngle));

		vPos = Vec3::Transform(vPos, xRot * yRot);

		Transform()->SetRelativePos(vPos);
	}
		break;

	case ImGuiMouseButton_Middle:
	{
		Vec3 zAxis = Vec3(0.f, 0.f, 1.f);
		zAxis = Vec3::TransformNormal(zAxis, Transform()->GetWorldMat());
		zAxis.Normalize();

		Vec3 vPos = Transform()->GetRelativePos();
		vPos.z += m_MouseDelta.y * 15.f;

		Transform()->SetRelativePos(vPos);
	}
		break;
	}

}
