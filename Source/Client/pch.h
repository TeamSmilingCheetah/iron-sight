#pragma once

#include "Common/global.h"
#include "Engine/System/Public/Manager/FLogManager.h"

#ifdef _DEBUG
#pragma comment(lib, R"(Engine/Engine_d)")
#else
#pragma comment(lib, R"(Engine/Engine)")
#endif

#ifdef _DEBUG
#pragma comment(lib, R"(Game/Game_d)")
#else
#pragma comment(lib, R"(Game/Game)")
#endif

#include <set>
using std::set;
