#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class PauseUIScript :
	public CScript
{
private:
	function<void()> m_ClickCallback;

	Ptr<FSound> m_HoverSound;
	Ptr<FSound> m_ClickSound;
	int			m_HoverSoundIdx;
	int			m_ClickSoundIdx;

public:

public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	virtual void OnMouseClick() override;
	virtual void OnMouseBeginHover() override;
	virtual void OnMouseEndHover() override;

public:
	CLONE(PauseUIScript);
	PauseUIScript();
	PauseUIScript(function<void()> _Callback);
	~PauseUIScript() override;
};

