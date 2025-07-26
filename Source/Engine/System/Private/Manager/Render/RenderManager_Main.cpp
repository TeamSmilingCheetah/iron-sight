#include "pch.h"
#include "Engine/System/Public/Manager/RenderManager.h"

#include "Engine/System/Public/Manager/CFontMgr.h"
#include "Engine/System/Public/Manager/CKeyMgr.h"
#include "Engine/System/Public/Manager/CTimeMgr.h"
#include "Engine/System/Public/Manager/CUIMgr.h"
#include "Engine/System/Public/Manager/CLevelMgr.h"
#include "Engine/System/Public/Rendering/Buffer/CConstBuffer.h"
#include "Engine/System/Public/Rendering/RenderPass/GeometryRenderPass.h"
#include "Engine/System/Public/Rendering/RenderPass/LightRenderPass.h"
#include "Engine/System/Public/Rendering/RenderPass/MergeRenderPass.h"
#include "Engine/Runtime/Public/Actor/CGameObject.h"
#include "Engine/Runtime/Public/Component/Camera/CCamera.h"
#include "Engine/Runtime/Public/Component/Transform/CTransform.h"
#include "Runtime/Public/Component/Light/CLight2D.h"
#include "Runtime/Public/Component/Light/CLight3D.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

FRenderManager::FRenderManager()
	: MultiRenderTargetArray{},
	  DebugObject(nullptr),
	  GeometryPass(nullptr),
	  LightPass(nullptr),
	  MergePass(nullptr),
	  bIsEditorMode(false),
	  bIsDebugRender(false),
	  bShowFPS(false),
	  EditorCamera(nullptr),
	  Light2DBuffer(new CStructuredBuffer),
	  Light3DBuffer(new CStructuredBuffer),
	  LightInstancingBuffer(new CStructuredBuffer),
	  PassParams()
{
}

FRenderManager::~FRenderManager()
{
	DELETE(GeometryPass);
	DELETE(LightPass);
	DELETE(MergePass);

	for (auto* RenderTarget : MultiRenderTargetArray)
	{
		DELETE(RenderTarget);
	}

	DELETE(Light2DBuffer);
	DELETE(Light3DBuffer);
	DELETE(LightInstancingBuffer);
}

/**
 * @brief 현재 활성화된 메인 카메라를 반환하는 함수
 */
CCamera* FRenderManager::GetMainCamera() const
{
	// Editor Mode
	if (bIsEditorMode)
	{
		return EditorCamera;
	}
	// Use Main Camera
	if (!CameraVector.empty())
	{
		return CameraVector[0];
	}

	LOG_ERROR("[Render][Camera] Can't Find Camera In Non Editor Mode");
	return nullptr;
}

void FRenderManager::Render()
{
	// SetUp By Build
#ifdef _DEBUG
	// Trigger Debug Render If Debug Build
	if (KEY_TAP(KEY::F9))
	{
		bIsDebugRender = !bIsDebugRender;
	}
#else
	if (bIsEditorMode)
	{
		bIsEditorMode = false;
	}
#endif

	if (KEY_TAP(KEY::F10))
	{
		bShowFPS = !bShowFPS;
	}

	RenderStart();

	if (bIsEditorMode)
	{
		RenderEditor();
	}
	else
	{
		RenderPlay();
	}

	RenderDebug();

	if (bShowFPS)
	{
		RenderFPS();
	}

	RenderClear();
}

void FRenderManager::RenderStart() const
{
	ClearMRT();
}

void FRenderManager::RenderDebug()
{
	if (!bIsDebugRender || DebugInfoList.empty())
	{
		return;
	}

	// Set Camera
	CCamera* Camera;
	if (bIsEditorMode)
	{
		Camera = EditorCamera;
	}
	else
	{
		if (!CameraVector.empty())
		{
			assert(!"메인 카메라가 없습니다");
		}

		Camera = CameraVector[0];
	}

	// Update Camera View & Projection
	UpdateViewProjection(Camera);

	// Render DebugShapes
	auto iter = DebugInfoList.begin();
	while (iter != DebugInfoList.end())
	{
		// Set Shape
		switch (iter->Shape)
		{
		case DEBUG_SHAPE::RECT:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
			DebugObject->MeshRender()->SetMaterial(DebugShapeMaterial, 0);
			break;
		case DEBUG_SHAPE::CIRCLE:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			DebugObject->MeshRender()->SetMaterial(DebugShapeMaterial, 0);
			break;
		case DEBUG_SHAPE::LINE:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
			DebugObject->MeshRender()->SetMaterial(DebugShapeLineMaterial, 0);
			// 라인 시작/끝점 정보 전달
			DebugObject->MeshRender()->GetMaterial(0)->SetScalarParam(VEC4_1, iter->WorldPos);
			DebugObject->MeshRender()->GetMaterial(0)->SetScalarParam(VEC4_2, iter->Scale);
			break;
		case DEBUG_SHAPE::CUBE:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh_Debug"));
			DebugObject->MeshRender()->SetMaterial(DebugShapeMaterial, 0);
			break;
		case DEBUG_SHAPE::SPHERE:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
			DebugObject->MeshRender()->SetMaterial(DebugShapeSphereMaterial, 0);
			break;
		case DEBUG_SHAPE::SKELETON:
			DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
			DebugObject->MeshRender()->SetMaterial(DebugSkeletonMaterial, 0);
			break;
		case DEBUG_SHAPE::CROSS:
			// XXX(KHJ): 이거 따로 설정할 필요 없는지?
			// Do Nothing
		default:
			DebugObject->MeshRender()->SetMaterial(DebugShapeMaterial, 0);
			break;
		}

		// 위치(Transform) 설정
		if (iter->matWorld == IdentityMatrix)
		{
			DebugObject->Transform()->SetRelativePos(iter->WorldPos);
			DebugObject->Transform()->SetRelativeScale(iter->Scale);
			DebugObject->Transform()->SetRelativeRotation(iter->Rotation);
			DebugObject->Transform()->FinalTick();
		}
		else
		{
			DebugObject->Transform()->SetWorldMat(iter->matWorld);
		}

		// Set Material
		Ptr<CMaterial> CurrentMaterial = DebugObject->MeshRender()->GetMaterial(0);
		CurrentMaterial->SetScalarParam(VEC4_0, iter->Color);

		if (iter->DepthTest)
		{
			CurrentMaterial->GetShader()->SetDSState(DS_TYPE::NO_WRITE);
		}
		else
		{
			CurrentMaterial->GetShader()->SetDSState(DS_TYPE::NO_TEST_NO_WRITE);
		}

		// Render
		if (iter->Shape != DEBUG_SHAPE::SKELETON)
		{
			DebugObject->Render();
		}
		else
		{
			// Skeleton Render
			auto* PureBoneBuffer = reinterpret_cast<CStructuredBuffer*>(iter->Data1);
			auto* ParentIdxBuffer = reinterpret_cast<CStructuredBuffer*>(iter->Data2);
			DebugObject->MeshRender()->Render_Skeleton(PureBoneBuffer, ParentIdxBuffer);
		}

		// Time Update & Erase Expired
		iter->Time += DT;
		if (iter->Duration <= iter->Time)
		{
			iter = DebugInfoList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void FRenderManager::RenderEditor()
{
	SetupFrameResources();
	ExecuteRenderPipeline(EditorCamera);
}

void FRenderManager::RenderPlay()
{
	if (CameraVector.empty())
	{
		LOG_TRACE("[Render][Play] No Camera Registered!");
		return;
	}

	SetupFrameResources();

	// TODO(KHJ): 카메라 타입 세팅
	for (size_t i = 0; i < CameraVector.size(); ++i)
	{
		CCamera* CurrentCamera = CameraVector[i];

		// 비활성화된 카메라는 렌더링하지 않음
		if (!CurrentCamera || !CurrentCamera->GetOwner()->IsActive() || !CurrentCamera->IsActvie())
		{
			continue;
		}

		// Camera View, Projection Binding
		UpdateViewProjection(CurrentCamera);

		// Main Camera
		if (i == 0)
		{
			ExecuteRenderPipeline(CurrentCamera);
		}

		// Minimap Camera
		else if (i == 1)
		{
			// XXX(KHJ): Main Camera에서 쓴 걸 지워주는 건가?
			MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)]->ClearRenderTargets();

			// Classify Objects
			ClassifyRenderObjects(CurrentCamera);

			// 미니맵용 G-Buffer 생성
			MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)]->BindTargets();
			GeometryPass->Execute(PassParams);

			// 미니맵용 MRT에 결과 Merge
			MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::MINIMAP)]->BindTargets();
			MergePass->ExecuteMinimap();
		}

		// UI Camera
		// TODO(KHJ): Camera에서 RenderManager로 역할 이관할 것
		else if (i == 2)
		{
			MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->BindTargets();
			CurrentCamera->render_ui();
			CurrentCamera->render_clear();
		}
	}
}

/**
 * @brief Frame 정보 출력을 위한 함수
 * Render에서 처리하지만 분류에 따라 여기에 위치했을 뿐인 Render의 구조를 전혀 활용하지 않는 Static
 */
void FRenderManager::RenderFPS()
{
	const auto& FPSInfo = CTimeMgr::GetInst()->GetFPSInfo();

	// FPS에 따른 색상 변경
	UINT color = FONT_RGBA(255, 20, 20, 255); // Red
	if (FPSInfo.first >= 60.f)
	{
		color = FONT_RGBA(20, 255, 20, 255); // Green
	}
	else if (FPSInfo.first >= 30.f)
	{
		color = FONT_RGBA(255, 255, 20, 255); // Yellow
	}

	CFontMgr::GetInst()->DrawFont(FPSInfo.second, 10, 20, 16, color);
}

void FRenderManager::RenderClear()
{
	CUIMgr::GetInst()->ClearUI();
}

void FRenderManager::ClearMRT() const
{
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->ClearAll();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)]->ClearRenderTargets();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::LIGHT)]->ClearRenderTargets();
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::MINIMAP)]->ClearAll();
}

/**
 * @brief Level의 Object를 전부 순회하면서 Frustum 판별 후, 내부 객체를 Material 단위로 해체하고 Command 단위로 모아놓는 함수
 */
void FRenderManager::ClassifyRenderObjects(CCamera* InCamera)
{
	RenderCommands.clear();

	vector<CGameObject*> GameObjects;
	CLevelMgr::GetInst()->GetCurrentLevel()->GetAllActiveObjectsInLevel(GameObjects);

	for (CGameObject* Object : GameObjects)
	{
		// TODO(KHJ): Frustum Culling 여기에서 처리할 수 있도록 할 것
		InCamera->SortObject(); // 해당 함수를 참고해서 수정해야 함

		CMeshRender* MeshRenderPtr = Object->MeshRender();
		if (!MeshRenderPtr)
		{
			continue;
		}

		Ptr<CMesh> MeshPtr = MeshRenderPtr->GetMesh();
		if (!MeshPtr.Get())
		{
			continue;
		}

		// Make Render Commands
		for (UINT i = 0; i < MeshRenderPtr->GetMaterialCount(); ++i)
		{
			Ptr<CMaterial> MaterialPtr = MeshRenderPtr->GetMaterial(i);
			if (!MaterialPtr.Get())
			{
				continue;
			}

			FRenderCommand Command = {};
			Command.Material = MaterialPtr.Get();
			Command.Mesh = MeshPtr.Get();
			Command.SectionIndex = i;
			Command.WorldMatrix = Object->Transform()->GetWorldMat();

			RenderCommands.push_back(Command);
		}
	}
}

void FRenderManager::ExecuteRenderPipeline(CCamera* InCamera)
{
	// Culling & Sorting
	ClassifyRenderObjects(InCamera);
	PassParams.RenderCommands = &RenderCommands;

	// Execute Geometry Pass
	// Opaque 오브젝트들을 G-Buffer에 작성
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::DEFERRED)]->BindTargets();
	GeometryPass->Execute(PassParams);

	// Execute Light Pass
	// G-Buffer와 Light 정보를 사용해 조명 연산을 수행
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::LIGHT)]->BindTargets();
	LightPass->Execute(PassParams);

	// Execute Merge Pass
	// Geometry Pass와 Light Pass의 결과를 합쳐 SwapChain에 그림
	MultiRenderTargetArray[static_cast<UINT>(MRT_TYPE::SWAPCHAIN)]->BindTargets();
	MergePass->Execute(PassParams);

	// TODO(KHJ): Forward, PostProcess, UI Render
}

/**
 * @brief 외부에서 디버그 렌더링을 요청할 때 사용하는 함수
 */
void FRenderManager::AddDebugShape(const tDebugShapeInfo& InDebugShapeInfo)
{
	DebugInfoList.push_back(InDebugShapeInfo);
}

/**
 * Constant Buffer의 Transform 영역에 현재 카메라의 View, Transform을 바인딩하는 Helper 함수
 * @param InCamera Setting이 필요한 카메라
 */
void FRenderManager::UpdateViewProjection(CCamera* InCamera)
{
	FViewProjectionInfo ViewProjectionInfo;

	CConstBuffer* ViewProjectionCB = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
	ViewProjectionInfo.View = InCamera->GetViewMat();
	ViewProjectionInfo.Projection = InCamera->GetProjMat();

	ViewProjectionCB->SetData(&ViewProjectionInfo);
	ViewProjectionCB->Binding();
}

void FRenderManager::RegisterCamera(CCamera* InCamera, UINT InIdx)
{
	if (CameraVector.size() <= InIdx)
	{
		CameraVector.resize(InIdx + 1);
	}

	if (CameraVector[InIdx])
	{
		LOG_ERROR_F("[Render][Camera] Camera Slot {} Is Already Taken. Overwriting.", InIdx);
	}

	CameraVector[InIdx] = InCamera;
}

void FRenderManager::UnregisterCamera(CCamera* InCamera)
{
	erase_if(CameraVector, [InCamera](const CCamera* Camera)
	{
		return Camera == InCamera;
	});
}

void FRenderManager::CopyRenderTarget() const
{
	CONTEXT->CopyResource(PostProcessTexture->GetTex2D().Get(), RenderTargetTexture->GetTex2D().Get());
}

void FRenderManager::SetupFrameResources()
{
	CConstBuffer* GlobalCB = CDevice::GetInst()->GetCB(CB_TYPE::GLOBAL);
	GlobalData GlobalInfo = {};
	GlobalInfo.Light2DCount = static_cast<int>(Light2DVector.size());
	GlobalInfo.Light3DCount = static_cast<int>(Light3DVector.size());
	GlobalCB->SetData(&GlobalInfo);
	GlobalCB->Binding();
	GlobalCB->Binding_CS();

	Light2DInfo.clear();
	Light3DInfo.clear();

	for (CLight2D* Light2D : Light2DVector)
	{
		Light2DInfo.push_back(Light2D->GetLight2DInfo());
	}

	if (Light2DBuffer->GetElementCount() < Light2DInfo.size())
	{
		Light2DBuffer->Create(sizeof(tLight2DInfo), static_cast<UINT>(Light2DInfo.size()), SRV_ONLY, true);
	}

	if (!Light2DInfo.empty())
	{
		Light2DBuffer->SetData(Light2DInfo.data(), static_cast<UINT>(Light2DInfo.size()));
		Light2DBuffer->Binding(13);
	}

	for (CLight3D* Light3D : Light3DVector)
	{
		Light3DInfo.push_back(Light3D->GetLight3DInfo());
	}

	if (Light3DBuffer->GetElementCount() < Light3DInfo.size())
	{
		Light3DBuffer->Create(sizeof(tLight3DInfo), static_cast<UINT>(Light3DInfo.size()), SRV_ONLY, true);
	}

	if (!Light3DInfo.empty())
	{
		Light3DBuffer->SetData(Light3DInfo.data(), static_cast<UINT>(Light3DInfo.size()));
		Light3DBuffer->Binding(14);
	}

	PassParams.Light2DInfos = &Light2DInfo;
	PassParams.Light3DInfos = &Light3DInfo;
}
