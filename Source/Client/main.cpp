#include "pch.h"
#include "Client/Core/Public/FClientApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	// Deprecated Params
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    FClientApp Client;
    return Client.Run(hInstance, nCmdShow);
}
