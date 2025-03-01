#pragma once

#include <Common/global.h>

#ifdef _DEBUG
#pragma comment(lib, R"(Engine/Engine_d)")
#else
#pragma comment(lib, R"(Engine/Engine)")
#endif

#ifdef _DEBUG
#pragma comment(lib, R"(Scripts/Scripts_d)")
#else
#pragma comment(lib, R"(Scripts/Scripts)")
#endif

#include <set>
using std::set;
