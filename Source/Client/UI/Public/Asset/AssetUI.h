#pragma once
#include "Common/Ptr.h"
#include "Client/UI/Public/Editor/EditorUI.h"
#include "Engine/System/Public/Asset/Base/CAsset.h"

class AssetUI :
	public EditorUI
{
private:
	Ptr<CAsset>			m_TargetAsset;
	const ASSET_TYPE	m_Type;
	bool				m_DirtyFlag;	// 선택 애셋이 바뀐 경우 true

public:
	void SetAsset(Ptr<CAsset> _Asset);
	Ptr<CAsset> GetAsset() { return m_TargetAsset; }

	bool IsDirty() const { return m_DirtyFlag; }

protected:
	void AssetTitle();
	void SaveButton();

	void Deactivate() override;

	virtual void Tick() override;

public:
	AssetUI(const string& _ID, ASSET_TYPE _Type);
	~AssetUI() override;
};
