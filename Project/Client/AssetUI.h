#pragma once
#include "EditorUI.h"

#include <Engine/Ptr.h>
#include <Engine/assets.h>

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
