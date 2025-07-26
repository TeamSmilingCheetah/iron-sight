#include "pch.h"
#include "Engine/System/Public/Rendering/RenderPass/GeometryRenderPass.h"

#include "Engine/System/Public/Manager/RenderManager.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"

using std::ranges::sort;

/**
 * @brief Geometry Render Pass 실행 함수
 * @param InParams RenderPass에 관여하는 변수 모음 Struct
 */
void FGeometryRenderPass::Execute(FRenderPassParameters& InParams)
{
	// Render Command Sorting
	const vector<FRenderCommand>& Commands = *InParams.RenderCommands;

	// Make Command Pointer Vector
	vector<const FRenderCommand*> CommandPointers;
	CommandPointers.reserve(Commands.size());

	for (const FRenderCommand& Command : Commands)
	{
		CommandPointers.push_back(&Command);
	}

	// Sorting With Pointer Vector
	sort(CommandPointers,
	     [](const FRenderCommand* CommandA, const FRenderCommand* CommandB)
	     {
		     // Material Sort
		     if (CommandA->Material != CommandB->Material)
		     {
			     return CommandA->Material < CommandB->Material;
		     }
		     // Mesh Sort
		     if (CommandA->Mesh != CommandB->Mesh)
		     {
			     return CommandA->Mesh < CommandB->Mesh;
		     }
		     // Index Sort
		     return CommandA->SectionIndex < CommandB->SectionIndex;
	     });

	// Batch Draw Call
	CConstBuffer* DrawCallCB = CDevice::GetInst()->GetCB(CB_TYPE::DRAWCALL);
	CMaterial* LastMaterial = nullptr;

	for (const FRenderCommand* Command : CommandPointers)
	{
		// Renew Binding If Material Changed
		if (Command->Material != LastMaterial)
		{
			Command->Material->Binding();
			LastMaterial = Command->Material;
		}

		// Constant Buffer Update
		FDrawCallInfo Info = {};
		Info.WorldMatrix = Command->WorldMatrix;
		DrawCallCB->SetData(&Info);
		DrawCallCB->Binding();

		// Draw Call
		Command->Mesh->Render(Command->SectionIndex);
	}
}
