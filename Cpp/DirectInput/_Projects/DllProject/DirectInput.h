#pragma once


//includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>


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
int joystickType = 0;

const wchar_t* hatDirections[] = { L"None",L"Up",L"Up-Right",L"Right",L"Down-Right",L"Down",L"Down-Left",L"Left",L"Up-Left" };

enum Axes {
	LStickX,
	LStickY,
	RStickX,
	RStickY,
	LeftTrigger,
	RightTrigger
};

enum Buttons {
	Cross = 0,
	A = 0,
	Circle = 1,
	B = 1,
};

struct Controller_Data {
	const char* m_Name;
	GUID m_Guid;
};

//conversion between a controller's values and what they represent on a normal controller
struct Controller_Buttons {
	union {
		struct {
			union {
				char m_A;
				char m_Cross;
			};
			union {
				char m_B;
				char m_Circle;
			};
			union {
				char m_X;
				char m_Square;
			};
			union {
				char m_Y;
				char m_Triangle;
			};
			union {
				char m_LeftBumper;
				char m_L1;
			};
			union {
				char m_RightBumper;
				char m_R1;
			};
			union {
				char m_LeftStick;
				char m_L3;
			};
			union {
				char m_RightStick;
				char m_R3;
			};
			union {
				char m_1RightStick;
				char m_1R3;
				char m_Start;
			};
			union {
				char m_2RightStick;
				char m_2R3;
				char m_Select;
			};
			char m_TouchPad;
			//ps button?!
		};
		char m_Buttons[32];//32 is from the DIJOYSTATE button array size
	};


	//Controller_Buttons(int* a) {
	//	memcpy(m_Buttons, a, 32);
	//}

	Controller_Buttons(std::vector<char> t) {
		for (int i = 0; i < t.size(); i++) {
			m_Buttons[i] = t[i];
		}
	}
};

#define MAP_AXES(x) ((int)FIELD_OFFSET(DIJOYSTATE,x))
#define MAP_BUTTON(x) ((int)FIELD_OFFSET(DIJOYSTATE,rgbButtons) + x)
struct Controller_Mapping {
	Controller_Data m_Base;

	//buttons
	Controller_Buttons m_Buttons;

	//axes
	//int m_Axes[6];// { MAP_AXES(lX),MAP_AXES(lY),MAP_AXES(lZ),MAP_AXES(lRx),MAP_AXES(lRy),MAP_AXES(lRz) };

	//pov is usually the same
	//being rgdwPOV[0]

	//Controller_Mapping(Controller_Data a_Cd,int a_NumButtons,int* a_Buttons,int a_NumAxes, int* a_Axes) 
	//	: m_Base(a_Cd) {
	//	//copy buttons
	//	for (int i = 0; i < 13; i++) {
	//		m_ButtonIndex[i] = a_Buttons[i];
	//	}
	//	//copy axes
	//	for (int i = 0; i < 6; i++) {
	//		m_Axes[i] = a_Axes[i];
	//	}
	//}
};

/*** FUNCTIONS */

/** non exported functions */

CALLBACK_FUNC enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context);
CALLBACK_FUNC enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
float getAxisFromEnum(Axes a_Axis);

//exported functions

// Link following functions C-style (required for plugins)
extern "C" {

	/* INPUT */

	//run this at the start of the program to start up the input manager
	const	EXPORT_API	int			startInput();
	//run this when closing the program to release the memory
	const	EXPORT_API	void		releaseInput();
	//this polls every connected joystick for updated information
	//call this before checking input
	const	EXPORT_API	int			updateInput();

	/* BUTTON/AXES */

	//returns a modifyed version based on the joystickType and commonControllers mappings
	const	EXPORT_API	int			getButton(int a_Index);
	//returns the normal version of which button has been pressed, based on controller index's
	const	EXPORT_API	int			getButtonNormal(int a_Index);
	const	EXPORT_API	int			getAxesValue(int a_Index);
	const	EXPORT_API	int			getPovValue();
	const	EXPORT_API	int			getPovDir();
	const	EXPORT_API	wchar_t*	getPovName(int a_Dir);

	const	EXPORT_API	int			getNumOfButtons();
	const	EXPORT_API	int			getNumOfAxis();
	const	EXPORT_API	int			getNumOfPov();

	const	EXPORT_API	wchar_t*	getButtonName(int a_Button);
	const	EXPORT_API	int			getButtonIndex(int a_Button);


	/* DEVICE INFORMATION */

	const	EXPORT_API	char*		getDeviceName();
	const	EXPORT_API	GUID		getDeviceGUID();

	const	EXPORT_API	int			getJoystickType();
} // end of export C block