#include "pch.h"
#include "Engine/System/Public/Manager/CAssetMgr.h"
#include "Engine/System/Public/Manager/CPathMgr.h"

CAssetMgr::CAssetMgr()
	: m_bAssetChanged(false)
{
}

CAssetMgr::~CAssetMgr()
{
}

void CAssetMgr::Shutdown()
{
    // 모든 에셋 컨테이너를 비워 참조를 해제하여 각 에셋의 소멸자가 호출되도록 한다.
    for (auto& assetBucket : m_mapAsset)
    {
        assetBucket.clear();
    }

    // 변경 플래그 초기화
    m_bAssetChanged = false;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _Key, UINT _Width, UINT _Height,
                                       DXGI_FORMAT _PixelFormat, UINT _BindFlag, D3D11_USAGE _Usage)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(_Key);

	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture;
	if (FAILED(pTex->Create(_Width, _Height, _PixelFormat, _BindFlag, _Usage)))
	{
		assert(nullptr);
		return nullptr;
	}

	pTex->SetKey(_Key);
	m_mapAsset[static_cast<UINT>(ASSET_TYPE::TEXTURE)].insert(make_pair(_Key, pTex.Get()));
	m_bAssetChanged = true;
	return pTex;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _Key, ComPtr<ID3D11Texture2D> _Tex2D)
{
	Ptr<CTexture> pTex = FindAsset<CTexture>(_Key);

	if (nullptr != pTex)
		return pTex;

	pTex = new CTexture;
	if (FAILED(pTex->Create(_Tex2D)))
	{
		assert(nullptr);
		return nullptr;
	}

	pTex->SetKey(_Key);
	m_mapAsset[static_cast<UINT>(ASSET_TYPE::TEXTURE)].insert(make_pair(_Key, pTex.Get()));
	m_bAssetChanged = true;
	return pTex;
}

void CAssetMgr::GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vecAssetNames)
{
	for (const auto& pair : m_mapAsset[static_cast<UINT>(_Type)])
	{
		_vecAssetNames.push_back(pair.first);
	}
}

void CAssetMgr::DeleteAsset(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<CAsset>>& mapAsset = m_mapAsset[static_cast<UINT>(_Type)];

	auto iter = mapAsset.find(_Key);

	if (mapAsset.end() == iter)
		return;

	mapAsset.erase(iter);

	m_bAssetChanged = true;
}

Ptr<CMeshData> CAssetMgr::LoadFBX(const wstring& _strPath)
{
	wstring strFileName = path(_strPath).stem();

	wstring strName = L"MeshData\\";
	strName += strFileName + L".mdat";

	Ptr<CMeshData> pMeshData = Load<CMeshData>(strName);

	if (nullptr != pMeshData)
		return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(_strPath);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);

	m_mapAsset[static_cast<UINT>(ASSET_TYPE::MESH_DATA)].
		insert(make_pair(strName, pMeshData.Get()));

	// meshdata 를 실제파일로 저장
	pMeshData->Save(strName);

	return pMeshData;
}

// map 수정 여부를 반환함
bool CAssetMgr::ChangeAssetKey(Ptr<CAsset> _Asset, const wstring& _NewKey)
{
	ASSET_TYPE type = _Asset->GetAssetType();
	UINT typeToIndex = static_cast<UINT>(type);
	auto iter = m_mapAsset[typeToIndex].find(_Asset->GetKey());

	// 아직 map에 등록되지 않은 경우
	if (iter == m_mapAsset[typeToIndex].end())
	{
		_Asset->SetKey(_NewKey);
		_Asset->SetRelativePath(_NewKey);

		return false;
	}

	// Engine 애셋 여부
	bool isEngineRes = _Asset->IsEngineAsset();

	// 파일로 존재하는 Asset은 파일 시스템에서도 변경해준다.
	if (!isEngineRes)
	{
		wstring Path = CPathMgr::GetInst()->GetContentPath();
		wstring curPath = Path + _Asset->GetKey();
		wstring newPath = Path + _NewKey;
		bool result = MoveFileEx(curPath.c_str(), newPath.c_str(), MOVEFILE_REPLACE_EXISTING);
		assert(result == true);
	}

	// map에 새로운 키값으로 등록
	AddAsset(_NewKey, _Asset);

	// FIXME(Ssio) : Relative Path랑 Key랑 동일하게 설정. 진짜 하나로 합치고 싶네 ㅅㅂ
	_Asset->SetRelativePath(_NewKey);

	// map에서 제거
	m_mapAsset[typeToIndex].erase(iter);

	// TODO(Ssio) : 이 Asset을 참조하던 다른 애셋에게도 알려줘야 함!!!!

	return true;
}

Ptr<CAsset> CAssetMgr::CopyAsset(Ptr<CAsset> _Source)
{
	Ptr<CAsset> pClone = _Source->Clone();

	if (pClone != nullptr)
	{
		ASSET_TYPE type = _Source->GetAssetType();
		UINT typeToIndex = static_cast<UINT>(type);
		const wstring& key = _Source->GetKey();

		// 확장자 탐색
		wstring ext = CPathMgr::GetInst()->GetFileExtension(key);
		wstring stem = CPathMgr::GetInst()->GetKeyWithoutExtension(key);

		// 사용되지 않은 id 값 찾기
		wchar_t buffer[4]{};
		UINT id = 0;

		// FIXME(Ssio) : 순회 방식 개선할 수 있을 거 같긴 함 (iterator 잘 써서 one pass로)
		while (true)
		{
			swprintf_s(buffer, L"%d", id);
			wstring newKey = stem + buffer + ext;

			if (m_mapAsset[typeToIndex].count(newKey))
			{
				++id;
			}
			else
			{
				AddAsset(newKey, pClone);
				pClone->SetRelativePath(newKey);
				break;
			}
		}
	}

	return pClone;
}
