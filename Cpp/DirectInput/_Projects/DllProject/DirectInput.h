#pragma once


//includes
#define DIRECTINPUT_VERSION 0x0800
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

//todo: move these variables to a struct so we can have more then 1 controller
LPDIRECTINPUTDEVICE8 joystick;
DIJOYSTATE2 joystickState;
DIDEVCAPS capabilities;
DIDEVICEINSTANCEA deviceInfo;

const wchar_t* hatDirections[] = { L"None",L"Up",L"Up-Right",L"Right",L"Down-Right",L"Down",L"Down-Left",L"Left",L"Up-Left" };

enum Axes {
	LStickX,
	LStickY,
	RStickX,
	RStickY,
	LeftTrigger,
	RightTrigger
};

struct Controller_Data {
	const char* m_Name;
	GUID m_CommonGuid;
};

//functions

//normal functions
CALLBACK_FUNC enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context);
CALLBACK_FUNC enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
float getAxisFromEnum(Axes a_Axis);

//exported functions

// Link following functions C-style (required for plugins)
extern "C" {

	const	EXPORT_API	int			startInput();
	const	EXPORT_API	void		releaseInput();
	const	EXPORT_API	int			updateInput();
	const	EXPORT_API	int			getButton(int a_Index);
	const	EXPORT_API	int			getAxesValue(int a_Index);
	const	EXPORT_API	int			getPovValue();
	const	EXPORT_API	int			getPovDir();
	const	EXPORT_API	wchar_t*	getPovName(int a_Dir);

	const	EXPORT_API	int			getNumOfButtons();
	const	EXPORT_API	int			getNumOfAxis();
	const	EXPORT_API	int			getNumOfPov();

	const	EXPORT_API	wchar_t*	getButtonName(int a_Button);

	const	EXPORT_API	char*		getDeviceName();
	const	EXPORT_API	GUID		getDeviceGUID();
} // end of export C block