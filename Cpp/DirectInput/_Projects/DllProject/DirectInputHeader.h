#pragma once


//includes
#include <dinput.h>


//defines
#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

#define CALLBACK_FUNC BOOL CALLBACK

//members/constants
#define MAX_AXES_VALUE 1000

LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joystick;
DIJOYSTATE2 joystickState;
DIDEVCAPS capabilities;

const wchar_t* hatDirections[] = { L"None",L"Up",L"Up-Right",L"Right",L"Down-Right",L"Down",L"Down-Left",L"Left",L"Up-Left" };

enum Axes {
	LStickX,
	LStickY,
	RStickX,
	RStickY,
	LeftTrigger,
	RightTrigger
};

//functions

//normal functions
CALLBACK_FUNC enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context);
CALLBACK_FUNC enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
float getAxisFromEnum(Axes a_Axis);

//exported functions

// Link following functions C-style (required for plugins)
extern "C" {

	int EXPORT_API startInput();
	void EXPORT_API releaseInput();
	int EXPORT_API updateInput();
	int EXPORT_API getButton(int index);
	int EXPORT_API getAxesValue(int index);
	int EXPORT_API getPovValue();
	int EXPORT_API getPovDir();
	const EXPORT_API wchar_t* getPovName(int a_Dir);

} // end of export C block