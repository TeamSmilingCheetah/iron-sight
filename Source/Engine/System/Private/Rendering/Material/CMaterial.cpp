#include "pch.h"
#include "System/Public/Rendering/Material/CMaterial.h"
#include "System/Public/Rendering/Buffer/CConstBuffer.h"
#include "System/Public/Rendering/Device/CDevice.h"

class CConstBuffer;

CMaterial::CMaterial(bool _EngineRes)
	: FAsset(MATERIAL, _EngineRes)
	  , m_Const{}
{
	m_Const.v4Arr[0] = Vec4(1.f, 0.f, 0.f, 1.f);
}

CMaterial::CMaterial(const CMaterial& _Origin)
	: FAsset(_Origin)
	  , m_Shader(_Origin.m_Shader)
	  , m_Const(_Origin.m_Const)
	  , m_arrTex{}
	  , m_SharedMtrl(_Origin.m_SharedMtrl)
{
	for (UINT i = 0; i < static_cast<UINT>(TEX_PARAM::TEX_END); ++i)
	{
		m_arrTex[i] = _Origin.m_arrTex[i];
	}
}

CMaterial::~CMaterial()
{
}

void CMaterial::Binding()
{
	if (nullptr == m_Shader)
		return;

	// Texture 바인딩
	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.bTex[i] = 0;
			CTexture::Clear(i);
			continue;
		}

		m_Const.bTex[i] = 1;
		m_arrTex[i]->Binding(i);
	}

	// 상수 데이터 바인딩
	static CConstBuffer* pMtrlCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();

	// Shader Binding
	m_Shader->Binding();
}

void CMaterial::Binding_Inst()
{
	if (nullptr == m_Shader)
		return;

	// Texture 바인딩
	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.bTex[i] = 0;
			CTexture::Clear(i);
			continue;
		}

		m_Const.bTex[i] = 1;
		m_arrTex[i]->Binding(i);
	}

	// 상수 데이터 바인딩
	static CConstBuffer* pMtrlCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();

	// Shader Binding
	m_Shader->BindingWithInstancing();
}

void* CMaterial::GetScalarParam(SCALAR_PARAM _Type)
{
	switch (_Type)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		return m_Const.iArr + _Type;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		return m_Const.fArr + (_Type - FLOAT_0);
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		return m_Const.v2Arr + (_Type - VEC2_0);
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		return m_Const.v4Arr + (_Type - VEC4_0);
	case MAT_0:
	case MAT_1:
		return m_Const.matArr + (_Type - MAT_0);
	}

	assert(nullptr);
	return nullptr;
}

void CMaterial::SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _tex)
{
	m_arrTex[_Param] = _tex;
}

CMaterial* CMaterial::Clone()
{
	auto pCloneMtrl = new CMaterial(*this);
	pCloneMtrl->m_SharedMtrl = this;
	return pCloneMtrl;
}

int CMaterial::Save(const wstring& _RelativePath)
{
	// m_SharedMtrl 원형 재질을 가리킨다 == 동적재질이다.
	// 동적 재질을 저장하려고 했다. ==> 에러
	// 동적 재질 = 게임이 플레이되는 도중에 일시적으로 만들어서 쓰고 버리는 재질
	assert(!m_SharedMtrl.Get());

	wstring strRelativePath = CPathMgr::GetInst()->MakeFileName(_RelativePath);
	SetRelativePath(strRelativePath);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + strRelativePath;


	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	SaveAssetRef(m_Shader, pFile);

	fwrite(&m_Const, sizeof(MtrlConst), 1, pFile);

	for (int i = 0; i < static_cast<int>(TEX_PARAM::TEX_END); ++i)
	{
		SaveAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}


int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
	assert(pFile);

	LoadAssetRef(m_Shader, pFile);

	fread(&m_Const, sizeof(MtrlConst), 1, pFile);

	for (int i = 0; i < static_cast<int>(TEX_PARAM::TEX_END); ++i)
	{
		LoadAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}
