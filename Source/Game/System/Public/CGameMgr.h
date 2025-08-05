#pragma once
#include "Common/singleton.h"

class CGameMgr
	: public singleton<CGameMgr>
{
	SINGLE(CGameMgr)

private:



public:
	int Init();

};
