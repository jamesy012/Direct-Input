#pragma once


//includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

#include "Controller.h"
#include "ExportHeader.h"

//defines
#define CALLBACK_FUNC BOOL CALLBACK

//members/constants


//todo: move these variables to a struct so we can have more then 1 controller
//reference to the direct input joystick object
LPDIRECTINPUTDEVICE8 joystick;
//data from the controller
DIJOYSTATE2 joystickState;
//how many output's does this controller have
DIDEVCAPS capabilities;
//information about this device
DIDEVICEINSTANCEA deviceInfo;
//which control scheme is this line
int joystickType = 0;


/*** FUNCTIONS */

/** non exported functions */

//counts the number of connected joysticks
CALLBACK_FUNC enumJoystickCountCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//will create a device for all connected joysticks
//currently just creates them for the first device it finds
CALLBACK_FUNC enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//sets up the axes for a joystick
CALLBACK_FUNC enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
//gets the axis value from Axes enum
float getAxisFromEnum(Axes a_Axis);

//exported functions

// Link following functions C-style (required for plugins)
EXPORT_START {

	/* INPUT */

	//run this at the start of the program to start up the input manager
	const	EXPORT_API	int			startInput();
	//run this when closing the program to release the memory
	const	EXPORT_API	void		releaseInput();
	//this polls every connected joystick for updated information
	//call this before checking input
	const	EXPORT_API	int			updateInput();

	/* BUTTON/AXES */

	//returns button information based on the controllers mapping index
	const	EXPORT_API	int			getButton(int a_Index);
	//returns button information based on the controllers index
	const	EXPORT_API	int			getButtonNormal(int a_Index);
	const	EXPORT_API	int			getAxesValue(int a_Index);
	const	EXPORT_API	int			getPovValue();
	const	EXPORT_API	int			getPovDir();
	const	EXPORT_API	wchar_t*	getPovName(int a_Dir);

	const	EXPORT_API	int			getNumOfButtons();
	const	EXPORT_API	int			getNumOfAxis();
	const	EXPORT_API	int			getNumOfPov();

	const	EXPORT_API	int			getButtonIndex(int a_Button);
	const	EXPORT_API	char*		getButtonNameConverted(int a_Button, bool a_IsXbox);

	/* DEVICE INFORMATION */

	const	EXPORT_API	char*		getDeviceName();
	const	EXPORT_API	GUID		getDeviceGUID();

	const	EXPORT_API	int			getJoystickType();
	const	EXPORT_API	char*		getKnownDeviceName();

} // end of export C block