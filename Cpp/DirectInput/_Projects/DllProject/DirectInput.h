#pragma once

//can probably put a lot of this in the cpp

//todo: support xInput to allow the xbox controllers to have separate triggers
//low priority 
//https://msdn.microsoft.com/en-us/library/windows/desktop/ee417014(v=vs.85).aspx

//todo have each controller store their index, and use that when getting input
//high priority 
//when having two controllers plugged in, and you plug in a new one,
//windows may give it index 1, so the controller already in 1 is now 2
//this effects the poll and GetDeviceState
//update the list to be in line with Windows's controller index
	//Update:
	//	controller's now keep their guid, tested with two ps4 controllers plugged in at the start
	//	Note:	when a controller is added later then it makes one controller act as two
	//			better hope that windows puts them in the right order if you add them after starting


//todo:	if the controller index is -1 then it will use the most recent controller
//medium priority 

//todo:	store the last controllers input and check to see how far it's moved
//low priority 

//todo: controller deadzone
//low priority 
	
//todo:	COMMENTS!
//high priority 

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
	//polls controller at index for input, if the poll fails, the controller will be made inactive
	const	EXPORT_API	int			updateInputController(int a_Index);
	//checks to see if there are any controllers that are connected, but haven't been checked yet
	//returns number of newly connected controllers
	const	EXPORT_API	int			updateControllers();

	/* CONTROLLER */

	//sets the current controller for the program,
	//use this before getting any information about a controller
	const	EXPORT_API	void		setCurrentController(int a_Index);
	//returns the number of controllers, only updates when a new controller is added
	//check if that controller is active
	const	EXPORT_API	int			getNumberOfControllers();
	//is this controller a xbox controller (note: atm this is defined only by guid, if the device has a different guid then it wont be a xbox controller)
	const	EXPORT_API	bool		isControllerXbox();
	//checks if this controller is active
	const	EXPORT_API	bool		isControllerActive();
	//index for this controller
	const	EXPORT_API	int			getControllerIndex();

	/* BUTTON/AXES */

	//runs the index through a re-mapper for this controller
	//use Buttons struct from Controller.h
	const	EXPORT_API	int			getButton(int a_Index);
	//returns true if button was pressed this frame
	//use Buttons struct from Controller.h
	const	EXPORT_API	int			getButtonDown(int a_Index);
	//returns true if button was released this frame
	//use Buttons struct from Controller.h
	const	EXPORT_API	int			getButtonUp(int a_Index);
	//just uses the button index to access the data,
	//eg 0 is square on ps4 controller while 0 is A on xbox controller
	//use getButton to have A and Cross map to the same
	const	EXPORT_API	int			getButtonNormal(int a_Index);
	//gets axis from controller
	//Right trigger and Left trigger can be merged into one axis
	const	EXPORT_API	float		getAxesValue(int a_Index);
	//returns the direct pov information from the controller
	const	EXPORT_API	int			getPovValue();
	//returns a number between 0 and 8
	//goes from 1 being up clockwise
	//2 will be up-right
	const	EXPORT_API	int			getPovDir();
	//returns the name for a direction, a_Dir can be between 0 and 8
	const	EXPORT_API	char*		getPovName(int a_Dir);

	//returns the number of buttons for the current controller
	const	EXPORT_API	int			getNumOfButtons();
	//returns the number of axis for the current controller
	const	EXPORT_API	int			getNumOfAxis();
	//returns the number of pov for the current controller
	const	EXPORT_API	int			getNumOfPov();

	//returns a re-mapped index based on the controller  
	const	EXPORT_API	int			getButtonIndex(int a_Button);
	//gets the name from the button index, it remaps it 
	const	EXPORT_API	char*		getButtonNameConverted(int a_Button, bool a_IsXbox);

	/* DEVICE INFORMATION */

	//gets device name from device
	const	EXPORT_API	char*		getDeviceName();
	//gets device GUID from device
	const	EXPORT_API	GUID		getDeviceGUID();
	//gets device instance GUID from device
	const	EXPORT_API	GUID		getDeviceInstanceGUID();

	//returns the type of device, maps directly to the array in ControllerMapping.h
	const	EXPORT_API	int			getJoystickType();
	//uses it's Joystick type name
	const	EXPORT_API	char*		getKnownDeviceName();

} // end of export C block