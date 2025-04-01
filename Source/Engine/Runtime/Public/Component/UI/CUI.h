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
	const UI_TYPE		m_UIType;		// Event 지원

	Vec2				m_LT;			// Left Top
	Vec2				m_RB;			// Right Bottom

	Vec4				m_BackGroundColor;

	UINT				m_Priority;		// Transform Z값으로 구현.
										// 같은 Canvas에 속하는 UI들은 (Priority-1, Priority] 범위를 사용함.

	// Image
	Ptr<CTexture>		m_Image;

	// Text
	vector<FontRenderInfo>		m_TextInfo;

public:
	void SetColor(Vec4 _Color) { m_BackGroundColor = _Color; }
	Vec4 GetColor() const { return m_BackGroundColor; }

	void SetImage(Ptr<CTexture> _Image) { m_Image = _Image; }
	Ptr<CTexture> GetImage() { return m_Image; }

	void SetPriority(UINT _Priority) {
		assert(_Priority >= 0);
		m_Priority = _Priority;
	}

	// 위치 설정
	void SetRectPos(Vec2 _Pos);
	void SetRectPos(float _x, float _y);

	Vec2 GetRectPos();

	void SetRectSize(Vec2 _Size);
	void SetRectSize(float _x, float _y);

	Vec2 GetRectSize();

	// Text 설정
	void AddText(const wstring& _Text, float _posX, float _posY, float _FontSize, UINT _Color);
	vector<FontRenderInfo>& GetTextInfoRef() { return m_TextInfo; }

	// Event 지원 여부 (bit masking)
	bool CanHover() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::HOVER); }
	bool CanClick() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::CLICK); }
	bool CanDrag() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::DRAG); }
	bool CanDrop() const { return static_cast<UINT>(m_UIType) & static_cast<UINT>(UI_TYPE::DROP); }


	void Binding();
	void FontRender();
	static void Clear();

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

