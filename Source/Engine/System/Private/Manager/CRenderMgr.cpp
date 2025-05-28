#include "pch.h"
#include "System/Public/Manager/CRenderMgr.h"
#include "Runtime/Public/Actor/CGameObject.h"
#include "Runtime/Public/Component/Camera/CCamera.h"
#include "Runtime/Public/Component/Light/CLight2D.h"
#include "Runtime/Public/Component/Light/CLight3D.h"
#include "Runtime/Public/Component/Rendering/CMeshRender.h"
#include "Runtime/Public/Component/Transform/CTransform.h"
#include "System/Public/Manager/CAssetMgr.h"
#include "System/Public/Manager/CKeyMgr.h"
#include "System/Public/Manager/CTimeMgr.h"
#include "System/Public/Manager/CUIMgr.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"
#include "System/Public/Rendering/Device/CDevice.h"
#include "System/Public/Rendering/RenderTarget/CMRT.h"

class CConstBuffer;

CRenderMgr::CRenderMgr()
	: m_arrMRT{}
	  , m_EditorCam(nullptr)
	  , m_Light2DBuffer(nullptr)
	  , m_DbgObj(nullptr)
	  , m_IsEditor(false)
	  , m_DebugRender(false)
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLight2DInfo), 2, SRV_ONLY, true);

	m_Light3DBuffer = new CStructuredBuffer;
	m_Light3DBuffer->Create(sizeof(tLight3DInfo), 2, SRV_ONLY, true);
}

CRenderMgr::~CRenderMgr()
{
	DELETE(m_DbgObj);
	DELETE(m_Light2DBuffer);
	DELETE(m_Light3DBuffer);

	for (UINT i = 0; i < static_cast<UINT>(MRT_TYPE::END); ++i)
	{
		DELETE(m_arrMRT[i]);
	}
}


void CRenderMgr::Render()
{
	if (KEY_TAP(KEY::F9))
	{
		m_DebugRender ? m_DebugRender = false : m_DebugRender = true;
	}

	// 렌더링 시작
	RenderStart();

	if (m_IsEditor)
	{
		Render_Editor();
	}
	else
	{
		Render_Play();
	}

	// DebugRender
	Render_Debug();

	// 리소스 클리어
	Render_Clear();
}

void CRenderMgr::RenderStart()
{
	ClearMRT();

	Binding();
}


void CRenderMgr::ClearMRT()
{
	m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->Clear();
	m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->ClearRenderTargets();
	m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->ClearRenderTargets();
}

void CRenderMgr::Binding()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL);

	g_Data.Light2DCount = static_cast<int>(m_vecLight2D.size());
	g_Data.Light3DCount = static_cast<int>(m_vecLight3D.size());

	pCB->SetData(&g_Data);
	pCB->Binding();
	pCB->Binding_CS();

	// 2D 광원 정보 바인딩
	static vector<tLight2DInfo> vecLight2DInfo;
	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2DInfo.push_back(m_vecLight2D[i]->GetLight2DInfo());
	}

	// 데이터를 받을 구조화버퍼의 크기가 모자라면, Resize 한다.
	if (m_Light2DBuffer->GetElementCount() < vecLight2DInfo.size())
	{
		m_Light2DBuffer->Create(sizeof(tLight2DInfo), static_cast<UINT>(vecLight2DInfo.size()),
		                        SRV_ONLY, true);
	}

	// 광원 1개 이상인 경우 구조화 버퍼로 데이터 이동
	if (!vecLight2DInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLight2DInfo.data(), static_cast<UINT>(vecLight2DInfo.size()));
		m_Light2DBuffer->Binding(13);
	}
	vecLight2DInfo.clear();


	// 3D 광원 정보 바인딩
	static vector<tLight3DInfo> vecLight3DInfo;
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLight3DInfo.push_back(m_vecLight3D[i]->GetLight3DInfo());
	}

	// 데이터를 받을 구조화버퍼의 크기가 모자라면, Resize 한다.
	if (m_Light3DBuffer->GetElementCount() < vecLight3DInfo.size())
	{
		m_Light3DBuffer->Create(sizeof(tLight3DInfo), static_cast<UINT>(vecLight3DInfo.size()),
		                        SRV_ONLY, true);
	}

	// 광원 1개 이상인 경우 구조화 버퍼로 데이터 이동
	if (!vecLight3DInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLight3DInfo.data(), static_cast<UINT>(vecLight3DInfo.size()));
		m_Light3DBuffer->Binding(14);
	}
	vecLight3DInfo.clear();
}


void CRenderMgr::Render_Debug()
{
	// Editor Mode
	if (m_vecCam.empty())
	{
		g_Trans.matView = m_EditorCam->GetViewMat();
		g_Trans.matProj = m_EditorCam->GetProjMat();
	}

	// Play Mode
	else
	{
		assert(m_vecCam[0]->GetOwner()->GetName() == L"MainCamera");

		g_Trans.matView = m_vecCam[0]->GetViewMat();
		g_Trans.matProj = m_vecCam[0]->GetProjMat();
	}

	auto iter = m_DbgList.begin();

	for (; iter != m_DbgList.end();)
	{
		if (m_DebugRender)
		{
			// 모양 설정
			switch (iter->Shape)
			{
			case DEBUG_SHAPE::RECT:
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"), 0);
				break;
			case DEBUG_SHAPE::CIRCLE:
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"), 0);
				break;
			case DEBUG_SHAPE::CROSS:
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"), 0);
				break;
			case DEBUG_SHAPE::LINE:
			{
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeLineMtrl"), 0);
				m_DbgObj->MeshRender()->GetMaterial(0)->
					SetScalarParam(VEC4_1, iter->WorldPos);
				m_DbgObj->MeshRender()->GetMaterial(0)->SetScalarParam(VEC4_2, iter->Scale);
			}
			break;
			case DEBUG_SHAPE::CUBE:
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh_Debug"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"), 0);
				break;
			case DEBUG_SHAPE::SPHERE:
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeSphereMtrl"), 0);
				break;
			case DEBUG_SHAPE::SKELETON:
				m_DbgObj->MeshRender()->SetMesh(
					CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
				m_DbgObj->MeshRender()->SetMaterial(
					CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugSkeletonMtrl"), 0);
				break;
			}

			// 위치설정
			if (iter->matWorld == XMMatrixIdentity())
			{
				m_DbgObj->Transform()->SetRelativePos(iter->WorldPos);
				m_DbgObj->Transform()->SetRelativeScale(iter->Scale);
				m_DbgObj->Transform()->SetRelativeRotation(iter->Rotation);
				m_DbgObj->Transform()->FinalTick();
			}
			else
			{
				m_DbgObj->Transform()->SetWorldMat(iter->matWorld);
			}

			// 색상 설정
			m_DbgObj->MeshRender()->GetMaterial(0)->SetScalarParam(VEC4_0, iter->Color);

			if (iter->DepthTest)
			{
				m_DbgObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSState(DS_TYPE::NO_WRITE);
			}
			else
			{
				m_DbgObj->MeshRender()->GetMaterial(0)->GetShader()->SetDSState(
					DS_TYPE::NO_TEST_NO_WRITE);
			}

			// 렌더
			if (iter->Shape != DEBUG_SHAPE::SKELETON)
				m_DbgObj->Render();
			else
			{
				CStructuredBuffer* pPureBoneBuffer = reinterpret_cast<CStructuredBuffer*>(iter->Data1);
				CStructuredBuffer* pParentIdxBuffer = reinterpret_cast<CStructuredBuffer*>(iter->Data2);

				m_DbgObj->MeshRender()->Render_Skeleton(pPureBoneBuffer, pParentIdxBuffer);
			}
		}

		iter->Time += EngineDT;
		if (iter->Duration <= iter->Time)
			iter = m_DbgList.erase(iter);
		else
			++iter;
	}
}

void CRenderMgr::Render_Play()
{
	// 레벨 내에 카메라로 레벨 렌더링
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		// 비활성화된 카메라는 렌더하지 않음.
		if (!m_vecCam[i]->GetOwner()->IsActive())
			continue;

		m_vecCam[i]->SortObject();

		g_Trans.matView = m_vecCam[i]->GetViewMat();
		g_Trans.matProj = m_vecCam[i]->GetProjMat();

		if (i == 0)
		{
			// Deferred
			m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->OMSet();
			m_vecCam[i]->render_deferred();

			// Decal
			m_arrMRT[static_cast<UINT>(MRT_TYPE::DECAL)]->OMSet();
			m_vecCam[i]->render_decal();

			// Lighting
			m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->OMSet();
			for (size_t i = 0; i < m_vecLight3D.size(); ++i)
			{
				m_vecLight3D[i]->Render();
			}


			// SwapChain
			m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->OMSet();


			// Deferred MRT 에 그려진 정보를 SwapChain 으로 이동
			MergeDeferredTarget();

			m_vecCam[i]->render_forward();
			m_vecCam[i]->render_particle();
			m_vecCam[i]->render_effect();
			m_vecCam[i]->render_transparent();
			m_vecCam[i]->render_postprocess();

			m_vecCam[i]->render_clear();


			// 특정 타겟으로 SwapChain 을 덮어쓰기
			MergeSpecifyTarget();		
		}
		else if (i == 1)
		{
			// SwapChain
			m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->OMSet();

			m_vecCam[i]->render_ui();
			m_vecCam[i]->render_clear();
		}
	}
	
}

void CRenderMgr::Render_Editor()
{
	m_EditorCam->SortObject();

	g_Trans.matView = m_EditorCam->GetViewMat();
	g_Trans.matProj = m_EditorCam->GetProjMat();

	// Deferred
	m_arrMRT[static_cast<UINT>(MRT_TYPE::DEFERRED)]->OMSet();
	m_EditorCam->render_deferred();

	// Decal
	m_arrMRT[static_cast<UINT>(MRT_TYPE::DECAL)]->OMSet();
	m_EditorCam->render_decal();

	// Lighting
	m_arrMRT[static_cast<UINT>(MRT_TYPE::LIGHT)]->OMSet();
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->Render();
	}

	// SwapChain
	m_arrMRT[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->OMSet();


	// Deferred MRT 에 그려진 정보를 SwapChain 으로 이동
	MergeDeferredTarget();



	m_EditorCam->render_forward();
	m_EditorCam->render_particle();
	m_EditorCam->render_effect();
	m_EditorCam->render_transparent();	
	m_EditorCam->render_postprocess();
	m_EditorCam->render_ui();

	m_EditorCam->render_clear();

	// 특정 타겟으로 SwapChain 을 덮어쓰기
	MergeSpecifyTarget();
}

void CRenderMgr::Render_Clear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();

	// UI 등록 해제
	CUIMgr::GetInst()->ClearUI();
}

void CRenderMgr::MergeDeferredTarget()
{
	Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
	m_MergeMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"ColorTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_2, CAssetMgr::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_3, CAssetMgr::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_4, CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex"));
	m_MergeMtrl->SetScalarParam(INT_0, 0);
	m_MergeMtrl->Binding();

	pRectMesh->Render(0);
	

	for (int i = 0; i < 8; ++i)
	{
		CTexture::Clear(i);
	}
}

void CRenderMgr::MergeSpecifyTarget()
{
	if (nullptr != m_SpecifyTarget)
	{
		Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
		m_MergeMtrl->SetTexParam(TEX_0, m_SpecifyTarget);
		m_MergeMtrl->SetScalarParam(INT_0, 1);
		m_MergeMtrl->Binding();

		pRectMesh->Render(0);

		for (int i = 0; i < 8; ++i)
		{
			CTexture::Clear(i);
		}
	}
}

void CRenderMgr::RegisterCamera(CCamera* _Cam, UINT _Priority)
{
	if (-1 == _Priority)
	{
		auto iter = m_vecCam.begin();
		for (; iter != m_vecCam.end(); ++iter)
		{
			if (*iter == _Cam)
			{
				m_vecCam.erase(iter);
				return;
			}
		}
	}

	else
	{
		if (m_vecCam.size() <= _Priority)
		{
			m_vecCam.resize(_Priority + 1);
		}

		assert(!m_vecCam[_Priority]);

		m_vecCam[_Priority] = _Cam;
	}
}


void CRenderMgr::CopyRenderTarget()
{
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_PostProcessTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}
