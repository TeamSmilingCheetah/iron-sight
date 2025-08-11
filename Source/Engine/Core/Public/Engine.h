#pragma once

class FEngine
	: public singleton<FEngine>
{
	SINGLE(FEngine)

private:
	HWND MainWindowHandle;
	Vec2 Resolution;

public:
	int Init(HWND InWindowHandle, UINT InWidth, UINT InHeight,
	         GAMEOBJECT_SAVE InSaveFunction, GAMEOBJECT_LOAD InLoadFunction);
	static void Progress();
	static void Shutdown();

	// TEST: memory profiling
	static void PrintMemoryUsage(const string& InText);

	// Getter
	HWND GetMainWindowHandle() const { return MainWindowHandle; }
};
