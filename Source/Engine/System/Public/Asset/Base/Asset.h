#pragma once
#include "Engine/Core/Public/CEntity.h"

class FAsset :
	public CEntity
{
private:
	wstring Key;
	wstring RelativePath;
	const ASSET_TYPE AssetType;
	int ReferenceCount;
	bool bIsEngineResource;

private:
	virtual int Load(const wstring& InFilePath) = 0;
	void Release();
	void AddReferenceCount();

public:
	// Friend Class
	template <typename T>
	friend class Ptr;

	friend class FAssetManager;
	friend class CFBXLoader;

	virtual int Save(const wstring& InFilePath) = 0;

	// Getter & Setter
	const wstring& GetKey() const { return Key; }
	const wstring& GetRelativePath() const { return RelativePath; }
	ASSET_TYPE GetAssetType() const { return AssetType; }
	int GetReferenceCount() const { return ReferenceCount; }
	bool IsEngineResource() const { return bIsEngineResource; }

	void SetKey(const wstring& InKey) { Key = InKey; }
	void SetRelativePath(const wstring& InPath) { RelativePath = InPath; }

	// Special Member Function
	FAsset(ASSET_TYPE InType, bool InIsEngineResource);
	FAsset(const FAsset& InOrigin);
	~FAsset() override;
	FAsset* Clone() override = 0;
};
