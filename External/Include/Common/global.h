#pragma once

#include <Windows.h>

// STL
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <typeinfo>

using std::vector;
using std::list;
using std::queue;
using std::priority_queue;
using std::map;
using std::unordered_map;
using std::make_pair;
using std::set;
using std::string;
using std::wstring;

#include <filesystem>
using namespace std::filesystem;


#include "singleton.h"

// DirectX 11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

// ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectxTex
#include <DirectxTex/DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectxTex/DirectXTex_debug")
#else
#pragma comment(lib, "DirectxTex/DirectXTex")
#endif

// Fbx Loader
#include <FBXLoader/fbxsdk.h>
#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")
#endif

// FMOD
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif

#include "SimpleMath.h"

#include "enum.h"
#include "define.h"
#include "struct.h"
#include "func.h"
#include "Ptr.h"
