#include <sdk/SexySDK.hpp>
#include <callbacks/callbacks.hpp>
#include <MinHook.h>
#include <utils/memory.hpp>

struct size2
{
	int width, height;
};

size2 resolution = { 1280, 960 };

float scale_width(float input)
{
	return (input / 800.f) * resolution.width;
}

float scale_height(float input)
{
	return (input / 600.f) * resolution.height;
}

float letterbox_width()
{
	return (800.f - resolution.width) / 2.f;
}

float letterbox_height()
{
	return (600.f - resolution.height) / 2.f;
}

static char* (__fastcall* Sexy__Graphics__DrawImage_)(Sexy::Graphics*, char*, Sexy::Image*, int, int);
char* __fastcall Sexy__Graphics__DrawImage(Sexy::Graphics* this_, char* edx, Sexy::Image* img, int x_pos, int y_pos)
{
	x_pos = (int)std::ceil(scale_height(x_pos));
	y_pos = (int)std::ceil(scale_height(y_pos));

	return Sexy__Graphics__DrawImage_(this_, edx, img, x_pos, y_pos);
}

static char* (__fastcall* Sexy__Graphics__DrawImageF_)(Sexy::Graphics*, char*, Sexy::Image*, float, float);
char* __fastcall Sexy__Graphics__DrawImageF(Sexy::Graphics* this_, char* edx, Sexy::Image* img, float x_pos, float y_pos)
{
	x_pos = scale_height(x_pos);
	y_pos = scale_height(y_pos);

	return Sexy__Graphics__DrawImageF_(this_, edx, img, x_pos, y_pos);
}

static void(__fastcall* Sexy__Graphics__DrawString_)(Sexy::Graphics*, char*, const std::string*, int, int);
void __fastcall Sexy__Graphics__DrawString(Sexy::Graphics* this_, char* edx, const std::string* str, int x_pos, int y_pos)
{
	Sexy__Graphics__DrawString_(this_, edx, str, x_pos, y_pos);
}

void init()
{
	MH_Initialize();

	MH_CreateHook((void*)0x004DBDC0, Sexy__Graphics__DrawImage, (void**)&Sexy__Graphics__DrawImage_);
	MH_CreateHook((void*)0x004D7C00, Sexy__Graphics__DrawImageF, (void**)&Sexy__Graphics__DrawImageF_);
	MH_CreateHook((void*)0x004CCFD0, Sexy__Graphics__DrawString, (void**)&Sexy__Graphics__DrawString_);

	//Window size
	set(0x00427034 + 0x6, resolution.width);
	set(0x0042703E + 0x6, resolution.height);

	MH_EnableHook(MH_ALL_HOOKS);
}

DWORD WINAPI OnAttachImpl(LPVOID lpParameter)
{
	init();
	return 0;
}

DWORD WINAPI OnAttach(LPVOID lpParameter)
{
	__try
	{
		return OnAttachImpl(lpParameter);
	}
	__except (0)
	{
		FreeLibraryAndExitThread((HMODULE)lpParameter, 0xDECEA5ED);
	}

	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, OnAttach, hModule, 0, nullptr);
		return true;
	}

	return false;
}
