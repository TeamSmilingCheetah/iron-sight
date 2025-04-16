#pragma once
#include "Engine/Runtime/Public/Actor/CGameObject.h"

class CGameObjectEx :
	public CGameObject
{
public:
	void FinalTick_Editor();
};
