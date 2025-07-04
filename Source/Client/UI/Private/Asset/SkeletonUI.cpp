#include "pch.h"
#include "Client/UI/Public/Asset/SkeletonUI.h"
#include "Client/UI/Public/Editor/TreeUI.h"

SkeletonUI::SkeletonUI()
	: AssetUI("Skeleton", SKELETON)
	, m_SelectedBoneIndex(-1)
{

}

SkeletonUI::~SkeletonUI()
{
}

//void SkeletonUI::Render_Update()
//{
//	AssetTitle();
//
//	Ptr<CSkeleton> pAsset = dynamic_cast<CSkeleton*>(GetAsset().Get());
//	assert(pAsset.Get());
//
//	// 뼈대 이름
//	const vector<tMTBone>* vecBorn = pAsset->GetBones();
//
//	for (int i = 0; i < vecBorn->size(); ++i)
//	{
//		string Borncnt = "Born " + std::to_string(i);
//		tMTBone data = (*vecBorn)[i];
//		auto BornName = WStringToString(data.strBoneName);
//
//		ImGui::InputText(Borncnt.c_str(), (char*)BornName.c_str(), BornName.length(),
//			ImGuiInputTextFlags_ReadOnly);
//	}
//
//}

void SkeletonUI::Render_Update()
{
	AssetTitle();

	Ptr<CSkeleton> pAsset = dynamic_cast<CSkeleton*>(GetAsset().Get());
	assert(pAsset.Get());

	const vector<tMTBone>* vecBones = pAsset->GetBones();
	if (vecBones->empty())
	{
		ImGui::Text("No bones found in skeleton");
		return;
	}

	// 툴바
	ImGui::Text("Bone Count: %d", static_cast<int>(vecBones->size()));
	ImGui::Separator();

	// 메인 컨텐츠
	if (ImGui::BeginTable("SkeletonLayout", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("Bone Hierarchy", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Bone Details", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		// 본 계층 구조
		for (int i = 0; i < vecBones->size(); ++i)
		{
			const tMTBone& bone = (*vecBones)[i];
			string boneName = WStringToString(bone.strBoneName);

			// 깊이
			string indent = "";
			for (int d = 0; d < bone.iDepth; ++d)
			{
				indent += "  ";
			}

			string displayName = indent + boneName + " (D:" + std::to_string(bone.iDepth) + ")";

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (m_SelectedBoneIndex == i)
				flags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx(displayName.c_str(), flags);

			if (ImGui::IsItemClicked())
				m_SelectedBoneIndex = i;
		}

		ImGui::TableSetColumnIndex(1);

		// 선택된 본 상세 정보
		if (m_SelectedBoneIndex >= 0 && m_SelectedBoneIndex < vecBones->size())
		{
			RenderBoneDetails(m_SelectedBoneIndex);
		}
		else
		{
			ImGui::Text("Select a bone to view details");
		}

		ImGui::EndTable();
	}
}

void SkeletonUI::RenderBoneDetails(int boneIndex)
{
	Ptr<CSkeleton> pAsset = dynamic_cast<CSkeleton*>(GetAsset().Get());
	const vector<tMTBone>* vecBones = pAsset->GetBones();
	const tMTBone& bone = (*vecBones)[boneIndex];

	// 기본 정보
	string boneName = WStringToString(bone.strBoneName);
	ImGui::Text("Name: %s", boneName.c_str());
	ImGui::Text("Index: %d", boneIndex);
	ImGui::Text("Depth: %d", bone.iDepth);
	ImGui::Text("Parent Index: %d", bone.iParentIndx);

	// 부모 본 이름
	if (bone.iParentIndx >= 0 && bone.iParentIndx < vecBones->size())
	{
		string parentName = WStringToString((*vecBones)[bone.iParentIndx].strBoneName);
		ImGui::Text("Parent: %s", parentName.c_str());
	}
	else
	{
		ImGui::Text("Parent: Root");
	}

	// 자식 본들
	vector<int> children;
	for (int i = 0; i < vecBones->size(); ++i)
	{
		if ((*vecBones)[i].iParentIndx == boneIndex)
		{
			children.push_back(i);
		}
	}

	if (!children.empty())
	{
		ImGui::Text("Children (%d):", static_cast<int>(children.size()));
		for (int childIndex : children)
		{
			string childName = WStringToString((*vecBones)[childIndex].strBoneName);
			ImGui::BulletText("%s", childName.c_str());
		}
	}
	else
	{
		ImGui::Text("Children: None");
	}

	ImGui::Separator();

	// Transform 정보
	RenderBoneTransform(bone.matBone, "Bone Transform");
	RenderBoneTransform(bone.matOffset, "Offset Transform");

	// Bind Pose 정보
	const vector<Matrix>& bindLocal = pAsset->GetBindLocal();
	if (boneIndex < bindLocal.size())
	{
		RenderBoneTransform(bindLocal[boneIndex], "Bind Local Transform");
	}
}

void SkeletonUI::RenderBoneTransform(const Matrix& matrix, const char* label)
{
	if (ImGui::CollapsingHeader(label))
	{
		ImGui::BeginGroup();

		// 분해된 Transform 정보 (위치, 회전, 스케일)
		XMVECTOR scale, rotation, translation;
		if (XMMatrixDecompose(&scale, &rotation, &translation, XMLoadFloat4x4(&matrix)))
		{
			XMFLOAT3 pos, scl;
			XMFLOAT4 rot;
			XMStoreFloat3(&pos, translation);
			XMStoreFloat4(&rot, rotation);
			XMStoreFloat3(&scl, scale);

			ImGui::Text("Position: (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
			ImGui::Text("Rotation: (%.3f, %.3f, %.3f, %.3f)", rot.x, rot.y, rot.z, rot.w);
			ImGui::Text("Scale: (%.3f, %.3f, %.3f)", scl.x, scl.y, scl.z);
		}

		ImGui::EndGroup();
	}
}
