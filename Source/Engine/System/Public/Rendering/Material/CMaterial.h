#pragma once
#include "Engine/System/Public/Asset/Base/CAsset.h"
#include "Engine/System/Public/Rendering/Shader/CGraphicShader.h"
#include "Engine/System/Public/Asset/Texture/CTexture.h"

class CMaterial :
	public CAsset
{
	Ptr<CGraphicShader> m_Shader;
	MtrlConst m_Const;
	Ptr<CTexture> m_arrTex[TEX_END];

	// 원본 재질을 가리키는 포인터(nullptr 인 경우 자신이 원본)
	Ptr<CMaterial> m_SharedMtrl;

public:
	template <typename T>
	void SetScalarParam(SCALAR_PARAM _Type, const T& _Data);
	void* GetScalarParam(SCALAR_PARAM _Type);

	void SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _tex);
	Ptr<CTexture>& GetTexParam(TEX_PARAM _Param) { return m_arrTex[_Param]; }

	void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
	{
		m_Const.mtrl.vDiff = _vDiff;
		m_Const.mtrl.vAmb = _vAmb;
		m_Const.mtrl.vSpec = _vSpec;
		m_Const.mtrl.vEmv = _vEmis;
	}

	void SetAnim3D(bool _bTrue) { m_Const.arrAnimData[0] = static_cast<int>(_bTrue); }
	void SetBoneCount(int _iBoneCount) { m_Const.arrAnimData[1] = _iBoneCount; }

	void operator =(const CMaterial& _OtherMtrl)
	{
		SetName(_OtherMtrl.GetName());

		m_Const = _OtherMtrl.m_Const;

		for (UINT i = 0; i < TEX_END; ++i)
		{
			m_arrTex[i] = _OtherMtrl.m_arrTex[i];
		}

		m_Shader = _OtherMtrl.m_Shader;
	}

	Ptr<CMaterial> GetSharedMtrl() { return m_SharedMtrl; }


	void SetShader(Ptr<CGraphicShader> _Shader) { m_Shader = _Shader; }
	Ptr<CGraphicShader> GetShader() { return m_Shader; }
	void Binding();
	void Binding_Inst();


	int Save(const wstring& _FilePath) override;

private:
	int Load(const wstring& _strFilePath) override;

public:
	CMaterial* Clone() override;

private:
	CMaterial(const CMaterial& _Origin);

public:
	CMaterial(bool _EngineRes = false);
	~CMaterial() override;
};


template <typename T>
void CMaterial::SetScalarParam(SCALAR_PARAM _Type, const T& _Data)
{
	bool bSet = false;

	switch (_Type)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, UINT>)
		{
			m_Const.iArr[_Type] = static_cast<int>(_Data);
			bSet = true;
		}

		break;

	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		if constexpr (std::is_same_v<T, float>)
		{
			m_Const.fArr[_Type - FLOAT_0] = _Data;
			bSet = true;
		}
		break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		if constexpr (std::is_same_v<T, Vec2>)
		{
			m_Const.v2Arr[_Type - VEC2_0] = _Data;
			bSet = true;
		}
		break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		if constexpr (std::is_same_v<T, Vec4> || std::is_same_v<T, Vec3>)
		{
			m_Const.v4Arr[_Type - VEC4_0] = _Data;
			bSet = true;
		}
		break;

	case MAT_0:
	case MAT_1:
		if constexpr (std::is_same_v<T, Matrix>)
		{
			m_Const.matArr[_Type - MAT_0] = _Data;
			bSet = true;
		}
		break;
	}

	assert(bSet);
}
