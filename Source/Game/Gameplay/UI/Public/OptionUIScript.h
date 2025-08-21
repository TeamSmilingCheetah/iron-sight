#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"


class OptionUIScript :
	public CScript
{
private:
	class PlayerCharacter* m_PlayerScript;

	function<void()> m_ClickCallback;

	Ptr<FSound> m_ClickSound;
	int			m_ClickSoundIdx;

public:
	void Begin() override;
	void Tick() override;


	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

	virtual void OnMouseClick() override;
	virtual void OnMouseBeginHover() override;
	virtual void OnMouseEndHover() override;

	void ApplyOption();

public:
	CLONE(OptionUIScript);
	OptionUIScript();
	OptionUIScript(function<void()> _Callback);
	~OptionUIScript() override;
};

