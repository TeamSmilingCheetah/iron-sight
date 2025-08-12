#include "pch.h"
#include "System/Public/Asset/Base/Asset.h"

FAsset::FAsset(ASSET_TYPE InType, bool InIsEngineResource)
	: Key(L"")
	  , RelativePath(L"")
	  , AssetType(InType)
	  , ReferenceCount(0)
	  , bIsEngineResource(InIsEngineResource)
{
}

FAsset::FAsset(const FAsset& InOrigin)
	: CEntity(InOrigin)
	  , Key(InOrigin.Key)
	  , RelativePath(InOrigin.RelativePath)
	  , AssetType(InOrigin.AssetType)
	  , ReferenceCount(0)
	  , bIsEngineResource(InOrigin.bIsEngineResource)
{
}

FAsset::~FAsset() = default;


void FAsset::AddReferenceCount() { ++ReferenceCount; }

void FAsset::Release()
{
	--ReferenceCount;

	if (ReferenceCount <= 0)
	{
		delete this;
	}
}
