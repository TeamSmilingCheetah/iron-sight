#pragma once
#include "UI/Public/Editor/EditorUI.h"
#include "Common/Ptr.h"
#include "System/Public/Asset/Base/CAsset.h"

class AssetUI :
	public EditorUI
{
	Ptr<CAsset> m_TargetAsset;
	const ASSET_TYPE m_Type;

public:
	void SetAsset(Ptr<CAsset> _Asset) { m_TargetAsset = _Asset; }
	Ptr<CAsset> GetAsset() { return m_TargetAsset; }

protected:
	void AssetTitle();

	void Deactivate() override;

public:
	AssetUI(const string& _ID, ASSET_TYPE _Type);
	~AssetUI() override;
};
