#pragma once

//can probably put a lot of this in the cpp

//todo: support xInput to allow the xbox controllers to have separate triggers
//https://msdn.microsoft.com/en-us/library/windows/desktop/ee417014(v=vs.85).aspx

//todo have each controller store their index, and use that when getting input
//when having two controllers plugged in, and you plug in a new one,
//windows may give it index 1, so the controller already in 1 is now 2
//this effects the poll and GetDeviceState
//update the list to be in line with Windows's controller index

//Update:
//	controller's now keep their guid, tested with two ps4 controllers plugged in at the start
//	Note:	when a controller is added later then it makes one controller act as two
//			better hope that windows puts them in the right order if you add them after starting

//includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


#include "ExportHeader.h"

//defines
#define CALLBACK_FUNC BOOL CALLBACK

//members/constants



/*** FUNCTIONS */

//non exported functions moved into cpp

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
	const	EXPORT_API	int			getControllerIndex();

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
	const	EXPORT_API	GUID		getDeviceInstanceGUID();

	const	EXPORT_API	int			getJoystickType();
	const	EXPORT_API	char*		getKnownDeviceName();

} // end of export C block