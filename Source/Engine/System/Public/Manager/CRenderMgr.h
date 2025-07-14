#pragma once
#include "Common/singleton.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"
#include "Engine/System/Public/Rendering/Material/CMaterial.h"

class CCamera;
class CLight2D;
class CLight3D;
class CStructuredBuffer;
class CMRT;

class CRenderMgr :
	public singleton<CRenderMgr>
{
	SINGLE(CRenderMgr);

private:
	CMRT* m_arrMRT[static_cast<UINT>(MRT_TYPE::END)];

	vector<CCamera*> m_vecCam; // 현재 레벨로 지정된 레벨 안에있는 카메라들
	CCamera* m_EditorCam; // 레벨에 속하지 않은 외부 카메라

	vector<CLight2D*> m_vecLight2D;
	CStructuredBuffer* m_Light2DBuffer;

	vector<CLight3D*> m_vecLight3D;
	CStructuredBuffer* m_Light3DBuffer;

	list<tDebugShapeInfo> m_DbgList; // 디버그 랜더링 정보
	class CGameObject* m_DbgObj; // 디버그 렌더링 오브젝트

	Ptr<CTexture> m_PostProcessTex; // 후처리용 텍스쳐(렌더타겟 복사용도)

	bool m_IsEditor; // 에디터모드 뷰 or 인게임 뷰

	Ptr<CTexture> m_SpecifyTarget; // 출력시킬 지정타겟
	Ptr<CMaterial> m_MergeMtrl;

	bool m_DebugRender;

public:
	void RegisterCamera(CCamera* _Cam, UINT _Priority);
	void RegisterEditorCamera(CCamera* _EditorCam) { m_EditorCam = _EditorCam; }
	void DeregisterCamera() { m_vecCam.clear(); }

	void RegisterLight2D(CLight2D* _Light2D) { m_vecLight2D.push_back(_Light2D); }

	int RegisterLight3D(CLight3D* _Light3D)
	{
		m_vecLight3D.push_back(_Light3D);
		return static_cast<int>(m_vecLight3D.size()) - 1;
	}

	void AddDebugShape(const tDebugShapeInfo& _info) { m_DbgList.push_back(_info); }

	void CopyRenderTarget();
	void SetEditorMode(bool _IsEditor) { m_IsEditor = _IsEditor; }
	bool IsEditorMode() const { return m_IsEditor; }

	void SetSpecifyTarget(Ptr<CTexture> _Target) { m_SpecifyTarget = _Target; }

	CCamera* GetMainCamera();
	CMRT* GetMRT(MRT_TYPE _Type) { return m_arrMRT[static_cast<UINT>(_Type)]; }

	void Init();
	void Render();

private:
	void RenderStart();
	void Binding();
	void ClearMRT();
	void Render_Debug();

	void Render_Play();
	void Render_Editor();

	void Render_Clear();

	void MergeDeferredTarget();
	void MergeSpecifyTarget();
	void MergeMinimapTarget();

	void CreateMRT();
	void CreateDebugMtrl();
	void CreateRenderMtrl();
};
