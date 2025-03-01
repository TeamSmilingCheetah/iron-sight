#pragma once

class CCopyBoneCS;
class CStructuredBuffer;

class CInstancingBuffer
	: public singleton<CInstancingBuffer>
{
	SINGLE(CInstancingBuffer);

private:
	ComPtr<ID3D11Buffer> m_InstancingBuffer;
	UINT m_MaxCount;
	vector<tInstancingData> m_vecData;

	// anim3D 용도
	vector<CStructuredBuffer*> m_vecBoneMat;
	int m_AnimInstCount;
	CStructuredBuffer* m_BoneBuffer;

	Ptr<CCopyBoneCS> m_CopyShader;

public:
	void Init();

	void Clear()
	{
		m_vecData.clear();
		m_vecBoneMat.clear();
		m_AnimInstCount = 0;
	}

	void AddInstancingData(tInstancingData& _tData) { m_vecData.push_back(_tData); }

	UINT GetInstanceCount() { return (UINT)m_vecData.size(); }
	ComPtr<ID3D11Buffer> GetBuffer() { return m_InstancingBuffer; }
	void SetData();

	void AddInstancingBoneMat(CStructuredBuffer* _pBuffer);
	int GetAnimInstancingCount() { return m_AnimInstCount; };
	CStructuredBuffer* GetBoneBuffer() { return m_BoneBuffer; }

private:
	void Resize(UINT _iCount);
};
