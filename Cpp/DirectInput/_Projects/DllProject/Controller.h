#pragma once

#include "ExportHeader.h"

//names of the different hat directions, clockwise
const wchar_t* hatDirections[] = { L"None",L"Up",L"Up-Right",L"Right",L"Down-Right",L"Down",L"Down-Left",L"Left",L"Up-Left" };

//const char* ControlNamesXbox[] = { "A", "B","X","Y","LB","RB","LS", "RS","Select","Start","Xbox" };
//const char* ControlNamesPs4[] = { "Cross", "Circle","Square","Triangle","L1","R1","L3", "R3","Select","Start","Ps", "L2","R2","Touchpad" };

const char* ControlNamesXbox[] = { "A", "B","X","Y","LB","RB","Select","Start","LS", "RS","Xbox" };
const char* ControlNamesPs4[] = { "Square", "Cross","Circle","Triangle","L1","R1","L2", "R2","Select","Start","L3","R3","Ps","Touchpad" };

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
	R1 = 5,
	RB = 5,
	L3 = 6,
	LS = 6,
	R3 = 7,
	RS = 7,
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
		for (int i = 0; i < a_Buttons.size(); i++) {
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
		for (int i = 0; i < a_Axes.size(); i++) {
			m_Axes[i] = a_Axes[i];
		}
		m_SingleTrigger = a_Tt;
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

EXPORT_START{

	const EXPORT_API char* getButtonName(int a_Index, bool a_XboxControls) {
		if (a_XboxControls) {
			return ControlNamesXbox[a_Index];
		} else {
			return ControlNamesPs4[a_Index];
		}
	}

}