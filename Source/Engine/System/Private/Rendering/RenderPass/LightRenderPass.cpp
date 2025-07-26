#include "pch.h"
#include "Engine/System/Public/Rendering/RenderPass/LightRenderPass.h"

#include "System/Public/Manager/RenderManager.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Buffer/CStructuredBuffer.h"

FLightRenderPass::FLightRenderPass()
	: LightInstancingBuffer(new CStructuredBuffer)
{
}

FLightRenderPass::~FLightRenderPass()
{
	DELETE(LightInstancingBuffer);
}

void FLightRenderPass::Execute(FRenderPassParameters& InParams)
{
    FRenderManager::GetInst()->GetRenderTarget(MRT_TYPE::DEFERRED)->BindTargets();

    RenderDirectionalLights(InParams);
    RenderPointLights(InParams);

	FRenderManager::GetInst()->GetRenderTarget(MRT_TYPE::DEFERRED)->ClearAll();
}

void FLightRenderPass::RenderDirectionalLights(const FRenderPassParameters& InParams)
{
	Ptr<CMaterial> DirectionalLightMaterial = GetMaterial(0);
	Ptr<CMesh> RectMesh = GetMesh(0);

    for (const tLight3DInfo& LightInfo : *InParams.Light3DInfos)
    {
        if (LightInfo.Type == LIGHT_TYPE::DIRECTIONAL)
        {
        	// GPU로 보낼 구조체를 만들고 CPU 데이터로 채운다.
        	FLightConstants LightConstant = {};
        	LightConstant.LightColor = Vec4(LightInfo.info.vColor, 1.f);
        	LightConstant.AmbientColor = Vec4(LightInfo.info.vAmbient, 1.f);
        	LightConstant.LightDirection = Vec4(LightInfo.Dir, 0.f);
        	LightConstant.Type = static_cast<int>(LightInfo.Type);

        	CConstBuffer* LightCB = CDevice::GetInst()->GetCB(CB_TYPE::LIGHT);
        	LightCB->SetData(&LightConstant);
        	LightCB->Binding();

            DirectionalLightMaterial->Binding();
            RectMesh->Render(0);
            return;
        }
    }
}

void FLightRenderPass::RenderPointLights(const FRenderPassParameters& InParams)
{
	Ptr<CMaterial> PointLightMaterial = GetMaterial(1);
	Ptr<CMesh> SphereMesh = GetMesh(1);

	vector<tLight3DInfo> PointLightInfos;
	for (const tLight3DInfo& LightInfo : *InParams.Light3DInfos)
	{
		if (LightInfo.Type == LIGHT_TYPE::POINT)
		{
			PointLightInfos.push_back(LightInfo);
		}
	}

	if (PointLightInfos.empty())
	{
		return;
	}

	// 2. 인스턴싱 버퍼 업데이트
	if (LightInstancingBuffer->GetElementCount() < PointLightInfos.size())
	{
		LightInstancingBuffer->Create(sizeof(tLight3DInfo), static_cast<UINT>(PointLightInfos.size()), SRV_ONLY, true);
	}
	LightInstancingBuffer->SetData(PointLightInfos.data(), static_cast<UINT>(PointLightInfos.size()));
	LightInstancingBuffer->Binding(15);

	PointLightMaterial->Binding();

	ComPtr<ID3D11BlendState> OneOneBS = CDevice::GetInst()->GetBSState(BS_TYPE::ONE_ONE);
	CONTEXT->OMSetBlendState(OneOneBS.Get(), nullptr, 0xffffffff);

	// 4. 단 한 번의 드로우콜로 모든 포인트 라이트 처리
	SphereMesh->RenderInstanced(LightInstancingBuffer, static_cast<UINT>(PointLightInfos.size()));

	// 5. 블렌딩 상태 원상 복구
	ComPtr<ID3D11BlendState> DefaultBS = CDevice::GetInst()->GetBSState(BS_TYPE::DEFAULT);
	CONTEXT->OMSetBlendState(DefaultBS.Get(), nullptr, 0xffffffff);
}
