#pragma once

#include <Engine/global.h>

#ifdef _DEBUG
#pragma comment(lib, "Engine\\Engine_d")
#else
#pragma comment(lib, "Engine\\Engine")
#endif


#ifdef _DEBUG
#pragma comment(lib, "Scripts\\Scripts_d")
#else
#pragma comment(lib, "Scripts\\Scripts")
#endif

#include <set>
using std::set;
