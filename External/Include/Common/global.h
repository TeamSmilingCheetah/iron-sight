#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

// STL
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <random>
#include <algorithm>
#include <array>
#include <variant>
#include <filesystem>
#include <functional>

using std::array;
using std::vector;
using std::list;
using std::queue;
using std::priority_queue;
using std::map;
using std::set;
using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::make_pair;
using std::string;
using std::wstring;
using std::min;
using std::max;
using std::unique_ptr;
using std::make_unique;
using std::variant;
using std::function;

namespace fs = std::filesystem;

// DirectX 11
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

// ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectxTex
#include <DirectxTex/DirectXTex.h>

// SimpleMath
#include "SimpleMath.h"

// Magic Enum
#include <magic_enum/magic_enum_all.hpp>

// FBX Loader
#include <FBXLoader/fbxsdk.h>

// FMOD
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

// Global Headers
#include "singleton.h"
#include "enum.h"
#include "define.h"
#include "struct.h"
#include "func.h"
#include "Ptr.h"

// Linking Library
#pragma comment(lib, DIRECTX_TEX_LIB)
#pragma comment(lib, FBX_LIB)
#pragma comment(lib, FMOD_LIB)
