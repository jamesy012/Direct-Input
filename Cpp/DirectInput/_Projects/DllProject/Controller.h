#pragma once

#include <vector>
#include "ExportHeader.h"

//names of the different hat directions, clockwise
const char* hatDirections[] = { "None","Up","Up-Right","Right","Down-Right","Down","Down-Left","Left","Up-Left" };

//const char* ControlNamesXbox[] = { "A", "B","X","Y","LB","RB","LS", "RS","Select","Start","Xbox" };
//const char* ControlNamesPs4[] = { "Cross", "Circle","Square","Triangle","L1","R1","L3", "R3","Select","Start","Ps", "L2","R2","Touchpad" };

const char* ControlNamesXbox[] = { "A", "B","X","Y","LB","RB","Select","Start","LS", "RS","Xbox" };
const char* ControlNamesPs4[] = { "Square", "Cross","Circle","Triangle","L1","R1","L2", "R2","Select","Start","L3","R3","Ps","Touchpad" };

enum Pov {
	None,
	Up,
	UpRight,
	Right,
	DownRight,
	Down,
	DownLeft,
	Left,
	UpLeft
};

enum Axes {
	LStickX,
	LStickY,
	RStickX,
	RStickY,
	//Left and Right Trigger can be the same
	LeftTrigger,
	RightTrigger
};

enum TriggerType {
	One,
	Two
};

enum Buttons {
	Cross = 0,
	A = 0,
	Circle = 1,
	B = 1,
	Square = 2,
	X = 2,
	Triangle = 3,
	Y = 3,
	L1 = 4,
	LB = 4,
	LeftBumper = 4,
	R1 = 5,
	RB = 5,
	RightBumper = 5,
	L3 = 6,
	LS = 6,
	LeftStick = 6,
	R3 = 7,
	RS = 7,
	RightStick = 7,
	Select = 8,
	Start = 9,
	Home = 10,
};

//information about this controller
struct Controller_Info {
	const char* m_Name;
	GUID m_Guid;
};

//conversion between a controller's values and what they represent on a normal controller
//32 possible buttons
//Based off Buttons Enum
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
			char m_Start;
			char m_Select;
			char m_Home;
		};
		char m_Buttons[32];//32 is from the DIJOYSTATE button array size
	};

	Controller_Buttons(std::vector<char> a_Buttons) {
		for (size_t i = 0; i < a_Buttons.size(); i++) {
			m_Buttons[i] = a_Buttons[i];
		}
	}

	char getButtonIndex(Buttons a_Button) {
		return m_Buttons[a_Button];
	}

};

struct Controller_Axis {
	union {
		struct {
			Axes m_LeftStickX;
			Axes m_LeftStickY;
			Axes m_RightStickX;
			Axes m_RightStickY;
			Axes m_LeftTrigger;
			Axes m_RightTrigger;
		};
		Axes m_Axes[6];
	};
	bool m_SingleTrigger = true;

	Controller_Axis(std::vector<Axes> a_Axes, TriggerType a_Tt) {
		for (size_t i = 0; i < a_Axes.size(); i++) {
			m_Axes[i] = a_Axes[i];
		}
		m_SingleTrigger = !!a_Tt;
	}
};

#define MAP_AXES(x) ((int)FIELD_OFFSET(DIJOYSTATE,x))
#define MAP_BUTTON(x) ((int)FIELD_OFFSET(DIJOYSTATE,rgbButtons) + x)
struct Controller_Data {
	Controller_Info m_Base;

	//list of buttons, these map to the Buttons enum
	Controller_Buttons m_Buttons;

	//axes
	Controller_Axis m_Axes;
	//int m_Axes[6];// { MAP_AXES(lX),MAP_AXES(lY),MAP_AXES(lZ),MAP_AXES(lRx),MAP_AXES(lRy),MAP_AXES(lRz) };
};

struct WindowsControllerData;
struct UserController;
struct InputInfo;

//the general idea here is swap the joystickState using currentState
//

struct InputInfo {
private:
	unsigned char currentState;

	unsigned int nextState() {
		return currentState ^ 1;
	}
public:
	DIJOYSTATE2 joystickState[2];

	DIJOYSTATE2* getNextState() {
		return &joystickState[nextState()];
	}

	DIJOYSTATE2* getLastState() {
		return &joystickState[nextState()];
	}

	DIJOYSTATE2* getCurrentState() {
		return &joystickState[currentState];
	}

	DIJOYSTATE2* updateState() {
		currentState = nextState();
		return getCurrentState();
	}
};


struct WindowsControllerData {
	//current and last frames DIJoyState2 data
	InputInfo joystickData;

	////data from the controller
	//DIJOYSTATE2 joystickState;

	//reference to the direct input joystick object
	LPDIRECTINPUTDEVICE8 joystick;

	//how many output's does this controller have
	DIDEVCAPS capabilities;
	//information about this device
	DIDEVICEINSTANCEA deviceInfo;

	//is this controller currently acquired
	bool acquired = false;

	UserController* userController = nullptr;
};

//all data based around a single controller
struct UserController {
	//which control scheme is this line
	int joystickType = 0;
	//this controllers index
	int controllerIndex = 0;

	//what was the last controller instance guid that this controller had
	GUID controllerGuid;

	WindowsControllerData* windowsController = nullptr;

};


EXPORT_START{

	const EXPORT_API char* getButtonName(int a_Index, bool a_XboxControls) {
		if (a_XboxControls) {
			return ControlNamesXbox[a_Index];
		} else {
			return ControlNamesPs4[a_Index];
		}
	}

}