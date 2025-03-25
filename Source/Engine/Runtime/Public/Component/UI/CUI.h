#pragma once
#include "Engine/Runtime/Public/Component/Base/CComponent.h"

enum class UI_TYPE
{
	// Event
	CLICK	= 0x00000001,
	HOVER	= 0x00000002,
	DRAG	= 0x00000004,	// Draggable Item
	DROP	= 0x00000008,	// Droppable Destination

	// Preset UI
	CANVAS	= 0x10000000,
	BUTTON	= 0x20000003,
	IMAGE	= 0x30000004,
	TEXT	= 0x40000002,

	END,
};

class CUI :
	public CComponent
{
	friend class CUIMgr;

private:
	const UI_TYPE		m_UIType;		// RTTI 구현

	Vec2				m_LT;			// Left Top
	Vec2				m_RB;			// Right Bottom
	Vec4				m_BackGroundColor;

	int					m_Priority;		// Transform Z값으로 구현.
										// 같은 Canvas에 속하는 UI들은 (Priority-1, Priority] 범위를 사용함.

	// Image
	Ptr<CTexture>		m_Image;

	// Text
	wstring				m_Text;

public:
	void SetColor(Vec4 _Color) { m_BackGroundColor = _Color; }
	Vec4 GetColor() const { return m_BackGroundColor; }

	void SetText(const wstring& _Text) { m_Text = _Text; }
	const wstring& GetText() const { return m_Text; }

	void SetPriority(int _Priority);


	// Event 지원 여부 (bit masking)
	bool CanClick() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::CLICK); }
	bool CanDrag() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::DRAG); }
	bool CanDrop() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::DROP); }



public:
	virtual void Begin() override;
	virtual void FinalTick() override;
	virtual void SaveComponent(FILE* _File) override;
	virtual void LoadComponent(FILE* _File) override;

public:
	CLONE(CUI);
	CUI(UI_TYPE _uiType);
	~CUI() override;
};

