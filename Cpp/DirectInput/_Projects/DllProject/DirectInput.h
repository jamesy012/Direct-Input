#pragma once

//can probably put a lot of this in the cpp

//includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

#include "Controller.h"
#include "ExportHeader.h"

//defines
#define CALLBACK_FUNC BOOL CALLBACK

//members/constants

//list of all controllers
std::vector<Controller*> m_Controllers;
int m_NumOfControllersFound = 0;

Controller* m_LatestController = nullptr;

HWND m_WindowHandle = NULL;

int m_ControllerIndex = 0;


/*** FUNCTIONS */

/** non exported functions */

//will create a device for all connected joysticks
//currently just creates them for the first device it finds
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
	//returns HRESULT from dInput calls
	const	EXPORT_API	int			startInput();
	//run this when closing the program to release the memory
	const	EXPORT_API	void		releaseInput();
	//this polls every connected joystick for updated information
	//call this before checking input
	//returns HRESULT from dInput calls
	const	EXPORT_API	int			updateInput();
	const	EXPORT_API	int			updateInputController(int a_Index);
	//checks to see if there are any controllers that are connected, but haven't been checked yet
	//returns number of newly connected controllers
	const	EXPORT_API	int			updateControllers();

	/* CONTROLLER */

	const	EXPORT_API	void		setCurrentController(int a_Index);
	const	EXPORT_API	int			getNumberOfControllers();
	const	EXPORT_API	bool		isControllerXbox();
	const	EXPORT_API	bool		isControllerActive();

	/* BUTTON/AXES */

	//returns button information based on the controllers mapping index
	const	EXPORT_API	int			getButton(int a_Index);
	//returns button information based on the controllers index
	const	EXPORT_API	int			getButtonNormal(int a_Index);
	const	EXPORT_API	int			getAxesValue(int a_Index);
	const	EXPORT_API	int			getPovValue();
	const	EXPORT_API	int			getPovDir();
	const	EXPORT_API	char*		getPovName(int a_Dir);

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