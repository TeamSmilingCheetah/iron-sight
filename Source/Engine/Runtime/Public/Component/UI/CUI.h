#pragma once
#include "Engine/Runtime/Public/Component/Base/Component.h"

enum UI_TYPE
{
	// Event
	UI_CLICK		= 0x00000001,
	UI_HOVER		= 0x00000002,
	UI_DRAG			= 0x00000004,	// Draggable Item
	UI_DROP			= 0x00000008,	// Droppable Destination
	UI_RIGHT_CLICK	= 0x00000010,	

	// Preset UI
	UI_CANVAS		= 0x10000000,
	UI_BUTTON		= 0x00000013,

	UI_DEFAULT		= 0x00000000,
};

class CUI :
	public FComponent
{
	friend class CUIMgr;

private:
	UINT				m_UIType;		// Event 지원

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
	void SetUIType(UINT _Type) { m_UIType = _Type; }
	UINT GetUIType() const { return m_UIType; }

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
	void ClearText() { m_TextInfo.clear(); }
	int GetTextvecSize() { return (int)m_TextInfo.size(); }
	void AddText(const wstring& _Text, float _posX, float _posY, UINT _FontSize, UINT _Color, const wstring& _Font = L"");
	void SetTextColor(int _idx, UINT _Color) { m_TextInfo[_idx].Color = _Color; }
	void SetTextSize(int _idx, UINT _FontSize) { m_TextInfo[_idx].FontSize = _FontSize; }
	void ModifyText(int idx, const wstring& _Text, float _posX, float _posY, UINT _FontSize, UINT _Color, const wstring& _Font = L"");
	vector<FontRenderInfo>& GetTextInfoRef() { return m_TextInfo; }

	// Event 지원 여부 (bit masking)
	bool CanHover() const { return m_UIType & UI_HOVER; }
	bool CanClick() const { return m_UIType & UI_CLICK; }
	bool CanRightClick() const { return m_UIType & UI_RIGHT_CLICK; }
	bool CanDrag() const { return m_UIType & UI_DRAG; }
	bool CanDrop() const { return m_UIType & UI_DROP; }


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
	CUI();
	CUI(UINT _uiType);
	~CUI() override;
};

