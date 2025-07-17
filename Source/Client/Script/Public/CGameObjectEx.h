#pragma once
#include "Engine/Runtime/Public/Actor/CGameObject.h"
class CComponent;

class CGameObjectEx
	: public CGameObject
{
public:
	void FinalTick_Editor();
	void Render_Editor();
	void FinalTick_Editor_Recur(CGameObject* _GameObject);
	void Render_Editor_Recur(CGameObject* _GameObject);

	void FinalTick_Component(CComponent* _Component);
};
