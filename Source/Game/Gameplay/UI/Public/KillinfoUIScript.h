#pragma once
#include "Engine/Runtime/Public/Component/Script/CScript.h"

class KillinfoUIScript :
	public CScript
{
private:
	CGameObject* m_Player;
	class PlayerCharacter* m_PlayerScript;


	float m_AccTime;

	bool m_bEvent;

	wstring m_KillerName;
	wstring m_KilledName;
	wstring m_KillWeapon;

public:
	void SetKillInfo(const wstring& _Killer, const wstring& _Killed, const wstring& _KillWeapon) { m_KillerName = _Killer; m_KilledName = _Killed; m_KillWeapon = _KillWeapon; }
	void OnEvent(); 

public:
	void Begin() override;
	void Tick() override;
	void SaveComponent(FILE* _File) override;
	void LoadComponent(FILE* _File) override;

public:
	CLONE(KillinfoUIScript);
	KillinfoUIScript();
	~KillinfoUIScript() override;
};

