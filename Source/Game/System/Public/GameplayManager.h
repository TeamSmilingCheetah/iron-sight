#pragma once

enum SCRIPT_TYPE
{
	CAMERASCRIPT,
	MISSILESCRIPT,
	PLAYERSCRIPT,

	WEAPONSCRIPT,
	GUNSCRIPT,
	THROWABLESCRIPT,

	TESTSOUND,

	ENEMYVISION,
	ENEMYCONTROLLER,
	TESTCHARACTER,

	// 아이템, 인벤토리 관련 스크립트
	INVENTORYSCRIPT,
	ITEMSCRIPT,		// Interactable
	ITEMUI,
	INVENTORYUI,
	VICINITYUI,

	// 상호작용
	INTERACTION_HANDLER,

	// 문
	DOORSCRIPT,		// Interactable
};

class CScript;

class GameplayManager
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript* GetScript(const wstring& _strScriptName);
	static CScript* GetScript(UINT _iScriptType);
	static const wchar_t* GetScriptName(CScript* _pScript);
};
