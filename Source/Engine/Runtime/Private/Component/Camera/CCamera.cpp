#include "pch.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Actor/CLevel.h"
#include "Engine/Runtime/Public/Component/Base/CRenderComponent.h"
#include "Engine/Runtime/Public/Component/Camera/CFrustum.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Engine/Runtime/Public/Component/UI/CUI.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Manager/CRenderMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Rendering/Device/CDevice.h"
#include "Engine/System/Public/Rendering/RenderTarget/CMRT.h"
#include "Engine/System/Public/Rendering/Buffer/CInstancingBuffer.h"
#include "Engine/Runtime/Public/Component/Animation/CAnimator3D.h"
#include "Engine/Runtime/Public/Component/Rendering/CMeshRender.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	  , m_Frustum(nullptr)
	  , m_ProjType(ORTHOGRAPHIC)
	  , m_Far(10000.f)
	  , m_AspectRatio(1.f)
	  , m_LayerCheck(0)
	  , m_Priority(-1)
	  , m_FOV(XM_PI / 2.f)
	  , m_Width(0)
	  , m_Scale(1.f)
{
	m_Frustum = new CFrustum;
	m_Frustum->SetOwner(this);

	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_Width = vResolution.x;
	m_AspectRatio = vResolution.x / vResolution.y;
}

CCamera::CCamera(const CCamera& _Origin)
	: CComponent(_Origin)
	  , m_Frustum(nullptr)
	  , m_ProjType(_Origin.m_ProjType)
	  , m_Far(_Origin.m_Far)
	  , m_AspectRatio(_Origin.m_AspectRatio)
	  , m_LayerCheck(_Origin.m_LayerCheck)
	  , m_Priority(_Origin.m_Priority)
	  , m_FOV(_Origin.m_FOV)
	  , m_Width(_Origin.m_Width)
	  , m_Scale(_Origin.m_Scale)
{
	m_Frustum = _Origin.m_Frustum->Clone();
	m_Frustum->SetOwner(this);
}

CCamera::~CCamera()
{
	if (nullptr != m_Frustum)
		delete m_Frustum;
}

void CCamera::Begin()
{
	// RenderMgr 에 등록
	CRenderMgr::GetInst()->RegisterCamera(this, m_Priority);
}

void CCamera::FinalTick()
{
	// ViewSpace
	// 1. 카메라가 있는곳이 원점
	// 2. 카메라가 바라보는 방향이 Z 축

	// View 행렬 계산
	Vec3 vWorldPos = Transform()->GetRelativePos();
	Matrix matTrans = XMMatrixTranslation(-vWorldPos.x, -vWorldPos.y, -vWorldPos.z);

	Matrix matRot = XMMatrixIdentity();

	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	matRot._11 = vR.x;
	matRot._12 = vU.x;
	matRot._13 = vF.x;
	matRot._21 = vR.y;
	matRot._22 = vU.y;
	matRot._23 = vF.y;
	matRot._31 = vR.z;
	matRot._32 = vU.z;
	matRot._33 = vF.z;

	m_matView = matTrans * matRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);


	// Proj 행렬 계산
	if (ORTHOGRAPHIC == m_ProjType)
		m_matProj = XMMatrixOrthographicLH(m_Width * m_Scale, (m_Width / m_AspectRatio) * m_Scale,
										   1.f, m_Far);
	else
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);

	// 마우스방향 Ray 계산
	CalcRay();

	// Frustum 생성
	m_Frustum->FinalTick();
}


void CCamera::SortObject()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (m_LayerCheck & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);

			const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

			for (size_t j = 0; j < vecObjects.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObjects[j]->GetRenderComponent();

				// 분류 예외조건 검사
				if (nullptr == pRenderCom || nullptr == pRenderCom->GetMesh())
				{
					continue;
				}

				// 오브젝트가 Frustum 내에 존재하지 않는다면 그대로 Pass
				if (!IsObjectInFrustum(vecObjects[j]))
				{
					continue;
				}

				// 머티리얼 개수만큼 반복
				UINT iMtrlCount = pRenderCom->GetMaterialCount();

				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// 재질이 없거나, 재질의 쉐이더가 설정이 안된 경우
					if (nullptr == pRenderCom->GetMaterial(iMtrl)
						|| nullptr == pRenderCom->GetMaterial(iMtrl)->GetShader())
					{
						continue;
					}

					// 쉐이더 도메인에 따른 분류
					Ptr<CGraphicShader> pShader = pRenderCom->GetMaterial(iMtrl)->GetShader();
					SHADER_DOMAIN Domain = pShader->GetDomain();

					switch (Domain)
					{
					case SHADER_DOMAIN::DOMAIN_DEFERRED:
					case SHADER_DOMAIN::DOMAIN_OPAQUE:
					case SHADER_DOMAIN::DOMAIN_MASK:
					{
						// Shader 의 DOMAIN 에 따라서 인스턴싱 그룹을 분류한다.
						map<ULONG64, vector<tInstObj>>* pMap = NULL;
						Ptr<CMaterial> pMtrl = pRenderCom->GetMaterial(iMtrl);

						if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
						{
							pMap = &m_mapInstGroup_D;
						}
						else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
							|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASK)
						{
							pMap = &m_mapInstGroup_F;
						}
						else
						{
							assert(nullptr);
							continue;
						}

						uInstID uID = {};
						uID.llID = pRenderCom->GetInstID(iMtrl);

						// ID 가 0 다 ==> Mesh 나 Material 이 셋팅되지 않았다.
						if (0 == uID.llID)
							continue;

						map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
						if (iter == pMap->end())
						{
							pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObjects[j], iMtrl }}));
						}
						else
						{
							iter->second.push_back(tInstObj{ vecObjects[j], iMtrl });
						}
					}
					break;
					case SHADER_DOMAIN::DOMAIN_DECAL:
						m_vecDecal.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
						m_vecTransparent.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_PARTICLE:
						m_vecParticle.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_EFFECT:
						m_vecEffect.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
						m_vecPostprocess.push_back(vecObjects[j]);
						break;
					case SHADER_DOMAIN::DOMAIN_UI:
						m_vecUI.push_back(vecObjects[j]);
						break;
					}
				}
			}
		}
	}
}

void CCamera::render_deferred()
{
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// Single Rendering 조건체크
		// 1. instancing 개수 조건 미만이거나
		// 2. FlipbookPlayer 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// 3. GraphicShader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->FlipbookPlayer()
			|| pair.second[0].pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			// ID 정보 추가 (이 부분이 빠져있었음!)
			tInstData.parentID = (int)pair.second[i].pObj->GetParentObjectID();
			tInstData.objectID = (int)pair.second[i].pObj->GetObjectID();

			if (pair.second[i].pObj->MeshRender()->IsSkinRender() && pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->Binding(pair.second[i].pObj->MeshRender());
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
				pObj = pair.second[i].pObj;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pObj->Animator3D()->GetBoneCount());
		}

		pMtrl->Binding_Inst();
		pMesh->Render_Object_Instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->Binding();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->Render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}

void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->Render();
	}
}

void CCamera::render_forward()
{
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Forward rendering (OPAQUE, MASK)
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// Single Rendering 조건체크
		// 1. instancing 개수 조건 미만이거나
		// 2. FlipbookPlayer 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// 3. GraphicShader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->FlipbookPlayer()
			|| pair.second[0].pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			// ID 정보 추가
			tInstData.parentID = (int)pair.second[i].pObj->GetParentObjectID();
			tInstData.objectID = (int)pair.second[i].pObj->GetObjectID();

			if (pair.second[i].pObj->MeshRender()->IsSkinRender() && pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->Binding(pair.second[i].pObj->MeshRender());
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
				pObj = pair.second[i].pObj;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pObj->Animator3D()->GetBoneCount());
		}

		pMtrl->Binding_Inst();
		pMesh->Render_Object_Instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->Binding();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->Render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}
void CCamera::render_transparent()
{
	// Transparent
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->Render();
	}
}

void CCamera::render_effect()
{
	// 렌더타겟 변경
	Ptr<CTexture> pEffectTarget = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EffectTargetTex");
	Ptr<CTexture> pEffectDepth = CAssetMgr::GetInst()->FindAsset<
		CTexture>(L"EffectDepthStencilTex");

	// 클리어
	CONTEXT->ClearRenderTargetView(pEffectTarget->GetRTV().Get(), Vec4(0.f, 0.f, 0.f, 0.f));
	CONTEXT->ClearDepthStencilView(pEffectDepth->GetDSV().Get(),
								   D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.f, 0);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<FLOAT>(pEffectTarget->GetWidth());
	viewport.Height = static_cast<FLOAT>(pEffectTarget->GetHeight());
	viewport.MaxDepth = 1.f;

	CONTEXT->RSSetViewports(1, &viewport);
	CONTEXT->OMSetRenderTargets(1, pEffectTarget->GetRTV().GetAddressOf(),
								pEffectDepth->GetDSV().Get());

	// Effect
	for (size_t i = 0; i < m_vecEffect.size(); ++i)
	{
		m_vecEffect[i]->Render();
	}

	// BlurTarget 으로 변경
	Ptr<CTexture> pEffectBlurTarget = CAssetMgr::GetInst()->FindAsset<CTexture>(
		L"EffectBlurTargetTex");
	Ptr<CMaterial> pBlurMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlurMtrl");
	Ptr<CMesh> pRectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");

	CONTEXT->ClearRenderTargetView(pEffectBlurTarget->GetRTV().Get(), Vec4(0.f, 0.f, 0.f, 0.f));

	CONTEXT->RSSetViewports(1, &viewport);
	CONTEXT->OMSetRenderTargets(1, pEffectBlurTarget->GetRTV().GetAddressOf(), nullptr);

	pBlurMtrl->SetTexParam(TEX_0, pEffectTarget);
	pBlurMtrl->Binding();
	pRectMesh->Render_Cluster_Instancing(2);

	CTexture::Clear(0);


	// 원래 렌더타겟으로 변경
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	Ptr<CMaterial> pEffectMergeMtrl = CAssetMgr::GetInst()->FindAsset<
		CMaterial>(L"EffectMergeMtrl");

	viewport.Width = static_cast<FLOAT>(pRTTex->GetWidth());
	viewport.Height = static_cast<FLOAT>(pRTTex->GetHeight());
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	CONTEXT->RSSetViewports(1, &viewport);
	CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

	pEffectMergeMtrl->SetTexParam(TEX_0, pEffectTarget);
	pEffectMergeMtrl->SetTexParam(TEX_1, pEffectBlurTarget);
	pEffectMergeMtrl->Binding();
	pRectMesh->Render(0);

	CTexture::Clear(0);
	CTexture::Clear(1);
}

void CCamera::render_particle()
{
	// 이제 실제 파티클을 그린다.
	for (size_t i = 0; i < m_vecParticle.size(); ++i)
	{
		m_vecParticle[i]->Render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostprocess.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPostprocess[i]->Render();
	}
}

void CCamera::render_ui()
{
	// m : canvas UI 개수
	// n : 총 UI 개수 (n >= m)
	// 총 O(m+n) <- 현재 구조를 해치지 않는 선에서 구현함
	const vector<CUI*> vecCanvasUI = CUIMgr::GetInst()->GetUIs();

	// UIMgr에서 Priority에 따라 정렬된 canvasOrder(인덱스)을 해쉬테이블에 등록 : O(m)
	unordered_map<CUI*, int> canvasOrder;
	for (int i = 0; i < vecCanvasUI.size(); ++i)
	{
		if (vecCanvasUI[i] == nullptr)
			continue;

		canvasOrder.emplace(vecCanvasUI[i], i);
	}

	// canvasUI를 m_vecUI에서 위치를 찾아서 canvasIdx에 등록함. O(n)
	vector<int> canvasIdx(vecCanvasUI.size());	// canvasIdx[i] : i번째 order의 canvas가 m_vecUI에서 등장하는 인덱스
	for (int i = 0; i < static_cast<int>(m_vecUI.size()); ++i)
	{
		if (canvasOrder.count(m_vecUI[i]->UI()))
			canvasIdx[canvasOrder[m_vecUI[i]->UI()]] = i;
	}

	// Priority 역순으로 canvasUI 인덱스부터 다음 canvasUI 직전까지 렌더함. O(n)
	for (int i = static_cast<int>(canvasIdx.size()) - 1; i >= 0; --i)
	{
		// ui가 존재하지 않아서 render 할 수 없는 상황에 render 호출하는 상황 방지
		if (m_vecUI.size() <= canvasIdx[i] + 1)
			continue;

		m_vecUI[canvasIdx[i]]->Render();

		for (size_t j = canvasIdx[i] + 1; j < m_vecUI.size(); ++j)
		{
			if (canvasOrder.count(m_vecUI[j]->UI()))
				break;

			m_vecUI[j]->Render();
		}
	}

	CUI::Clear();
}


void CCamera::render_clear()
{
	// 이전 프레임 분류정보 제거
	m_mapInstGroup_D.clear();
	m_mapInstGroup_F.clear();

	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecEffect.clear();
	m_vecParticle.clear();
	m_vecPostprocess.clear();
	m_vecUI.clear();
}

void CCamera::LayerCheck(int _LayerIdx)
{
	if (m_LayerCheck & (1 << _LayerIdx))
	{
		m_LayerCheck &= ~(1 << _LayerIdx);
	}

	else
	{
		m_LayerCheck |= (1 << _LayerIdx);
	}
}

/**
 * @brief 오브젝트 1개에 대해서 frustum 내부에 있는지 판별하는 함수
 *
 * @param _Object
 * @return
 */
bool CCamera::IsObjectInFrustum(const CGameObject* _Object)
{
	if (!_Object->Transform()->FrustumCheckRequired())
	{
		return true;
	}

	// UI 컴포넌트 보유 시 frustum 체크 안함
	if (_Object->UI())
	{
		return true;
	}

	// 오브젝트의 바운딩 박스 가져오기
	// TODO(KHJ): 현재 bounding box 계산하지 못했다면 무조건 rendering하지 않는데, 누락되는 케이스 없는지 확인 필요
	Vec3 vMin, vMax;
	if (!_Object->CalculateBoundingBox(vMin, vMax))
	{
		return false;
	}

	// AABB의 8개 꼭지점
	Vec3 vPoints[8] = {
		Vec3(vMin.x, vMin.y, vMin.z), // 0: 좌하단
		Vec3(vMax.x, vMin.y, vMin.z), // 1: 우하단
		Vec3(vMax.x, vMax.y, vMin.z), // 2: 우상단
		Vec3(vMin.x, vMax.y, vMin.z), // 3: 좌상단
		Vec3(vMin.x, vMin.y, vMax.z), // 4: 좌하단(후면)
		Vec3(vMax.x, vMin.y, vMax.z), // 5: 우하단(후면)
		Vec3(vMax.x, vMax.y, vMax.z), // 6: 우상단(후면)
		Vec3(vMin.x, vMax.y, vMax.z)  // 7: 좌상단(후면)
	};

	// AABB의 8개 꼭지점 중 하나라도 프러스텀 내부에 있으면 렌더링
	for (int i = 0; i < 8; ++i)
	{
		if (m_Frustum->IsInFrustum(vPoints[i]))
			return true;
	}

	return false;
}

void CCamera::CalcRay()
{
	// ViewPort 정보
	CMRT* pSwapChainMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	if (nullptr == pSwapChainMRT)
		return;

	// 메인 MRT 의 ViewPort 값을 가져온다.
	const D3D11_VIEWPORT& VP = pSwapChainMRT->GetViewPort();

	// 현재 마우스 좌표
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// 마우스를 향하는 직선은 카메라 위치를 지난다.
	m_Ray.vStart = Transform()->GetWorldPos();

	// View 공간 상에서 카메라에서 마우스 방향을 향하는 방향벡터를 구한다.
	//  - 마우스가 있는 좌표를 -1 ~ 1 사이의 정규화된 좌표로 바꾼다.
	//  - 투영행렬의 _11, _22 에 있는 값은 Near 평면상에서 Near 값을 가로 세로 길이로 나눈값
	//  - 실제 ViewSpace 상에서의 Near 평명상에서 마우스가 있는 지점을 향하는 위치를 구하기 위해서 비율을 나누어서
	//  - 실제 Near 평면상에서 마우스가 향하는 위치를 좌표를 구함
	m_Ray.vDir.x = (((vMousePos.x - VP.TopLeftX) * 2.f / VP.Width) - 1.f) / m_matProj._11;
	m_Ray.vDir.y = -(((vMousePos.y - VP.TopLeftY) * 2.f / VP.Height) - 1.f) / m_matProj._22;
	m_Ray.vDir.z = 1.f;

	// 방향 벡터에 ViewMatInv 를 적용, 월드상에서의 방향을 알아낸다.
	m_Ray.vDir = XMVector3TransformNormal(m_Ray.vDir, m_matViewInv);
	m_Ray.vDir.Normalize();
}

void CCamera::SaveComponent(FILE* _File)
{
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
	fwrite(&m_Priority, sizeof(int), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_Scale, sizeof(float), 1, _File);
}

void CCamera::LoadComponent(FILE* _File)
{
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
	fread(&m_Priority, sizeof(int), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_Scale, sizeof(float), 1, _File);
}
