#pragma once

class CEntity
{
	static UINT g_NextID;

	wstring m_Name;
	UINT m_ID;

public:
	void SetName(const wstring& _Name) { m_Name = _Name; }
	const wstring& GetName() const { return m_Name; }

	UINT GetID() const { return m_ID; }

	virtual void SaveToLevel(FILE* _File);
	virtual void LoadFromLevel(FILE* _File);

	virtual CEntity* Clone() = 0;

	CEntity();
	CEntity(const CEntity& _Origin);
	virtual ~CEntity();
};
