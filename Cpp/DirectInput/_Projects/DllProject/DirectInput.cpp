
#include "DirectInput.h"
#include "ControllerMapping.h"

#define NUM_OF_COMMON_CONTROLLER_TYPES 4
//if two controllers use the same mapping then there will be duplicated data :/
//might change is so

//this represents the controller name/guid
//and which button corresponds to a standardized mapping
const Controller_Data CommonControllers[NUM_OF_COMMON_CONTROLLER_TYPES] = {
	Controller_Data{//this is the default controller
	/*guid*/	Controller_Info{ "UNKNOWN",GUID{ 0,0,0,{ 0,0,0,0,0,0,0,0 } } },
	/*buttons*/ Controller_Buttons{ { 0,1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,32,33,34,35 } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::RStickX,Axes::RStickY,Axes::LeftTrigger },TriggerType::Two }
	},
	Controller_Data{
	/*guid*/	Controller_Info{ "PS4 Controller",GUID{ 96732492,0,0,{ 0,0,80,73,68,86,73,68 } } },
	/*buttons*/ Controller_Buttons{{ 1, 2, 0, 3, PS4_L1, PS4_R1,PS4_L3, PS4_R3, PS4_SELECT, PS4_START,PS4_PS, PS4_L2, PS4_R2,PS4_TOUCH_PAD, }},
	/*Axes*/	Controller_Axis{ {Axes::LStickX,Axes::LStickY,Axes::RStickX,Axes::RStickY,Axes::LeftTrigger,Axes::RightTrigger},TriggerType::Two }
	},
	Controller_Data{
	/*guid*/	Controller_Info{ "Game pad F310", GUID{ 3256681581,0,0,{ 0, 0, 80, 73, 68, 86, 73, 68 } } },
	/*buttons*/ Controller_Buttons{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::LeftTrigger,Axes::RightTrigger,Axes::LStickX },TriggerType::One }
	},
	Controller_Data{
	/*guid*/	Controller_Info{ "Xbox one controller", GUID{ 47252574,0,0,{ 0, 0, 80, 73, 68, 86, 73, 68 } } },
	/*buttons*/ Controller_Buttons{ { 0, 1, 2, 3, 4, 5, 8, 9, 6, 7, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::LeftTrigger,Axes::RightTrigger,Axes::RStickX,Axes::RStickX },TriggerType::One }
	},
};

//members/constants
#define MAX_AXES_VALUE 1000

//this is the direct input object used through out the class
LPDIRECTINPUT8 di;


const EXPORT_API int startInput() {
	if (di != nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	HWND handle = GetActiveWindow();
	if (handle == NULL) {
		handle = GetConsoleWindow();
	}

	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&di, NULL);
	if (FAILED(hr)) {
		return hr;
	}

	hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickSelectCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		return hr;
	}

	if (joystick == NULL) {
		return E_FAIL;
	}

	hr = joystick->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) {
		return hr;
	}


	hr = joystick->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		return hr;
	}

	capabilities.dwSize = sizeof(DIDEVCAPS);
	hr = joystick->GetCapabilities(&capabilities);
	if (FAILED(hr)) {
		return hr;
	}

	hr = joystick->EnumObjects(enumAxesSetCallback, NULL, DIDFT_AXIS);
	if (FAILED(hr)) {
		return hr;
	}



	deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEA);
	hr = joystick->GetDeviceInfo(&deviceInfo);
	if (FAILED(hr)) {
		return hr;
	}

	//DIDEVICEOBJECTDATA rgdod[10];
	//DWORD dwItems = 10;
	//hr = joystick->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
	//if (FAILED(hr)) {
	//	return hr;
	//}

	return S_OK;
}

const EXPORT_API void releaseInput() {
	if (joystick) {
		joystick->Unacquire();
		joystick->Release();
		joystick = nullptr;
	}
	di->Release();
	di = nullptr;
}

const EXPORT_API int updateInput() {
	HRESULT hr;

	if (joystick == NULL) {
		return S_OK;
	}

	hr = joystick->Poll();
	if (FAILED(hr)) {
		//lost connection to the joystick

		do {
			hr = joystick->Acquire();
		} while (hr == DIERR_INPUTLOST);

		if (hr == DIERR_INVALIDPARAM || hr == DIERR_NOTINITIALIZED) {
			return E_FAIL;
		}

		if (hr == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
	}

	hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), &joystickState);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

const EXPORT_API int getButton(int a_Index) {
	if (joystick) {
		if (a_Index >= capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		int index = getButtonIndex(a_Index);
		return joystickState.rgbButtons[index];
	}
	return 0;
}

const EXPORT_API int getButtonNormal(int a_Index) {
	if (joystick) {
		if (a_Index >= capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		return joystickState.rgbButtons[a_Index];
	}
	return 0;
}

const EXPORT_API int getAxesValue(int a_Index) {
	if (joystick) {
		Axes index = CommonControllers[joystickType].m_Axes.m_Axes[a_Index];
		return getAxisFromEnum(index);
	}
	return 0;
}

const EXPORT_API int getPovValue() {
	if (joystick) {
		return joystickState.rgdwPOV[0];
	}
	return 0;
}

const EXPORT_API int getPovDir() {
	int hat = 0;
	if (joystick) {
		hat = ((int)joystickState.rgdwPOV[0] / 4500) + 1;
		if (joystickState.rgdwPOV[0] == -1 || (unsigned int)hat >= 10) {
			hat = 0;
		}
	}
	return hat;
}

const EXPORT_API wchar_t* getPovName(int a_Dir) {
	if (joystick) {
		if (a_Dir >= 9 || a_Dir <= -1) {
			a_Dir = 0;
		}
		return hatDirections[a_Dir];
	}
	return hatDirections[0];
}

const EXPORT_API int getNumOfButtons() {
	if (joystick) {
		return capabilities.dwButtons;
	}
	return 0;
}

const EXPORT_API int getNumOfAxis() {
	if (joystick) {
		return capabilities.dwAxes;
	}
	return 0;
}

const EXPORT_API int getNumOfPov() {
	if (joystick) {
		return capabilities.dwPOVs;
	}
	return 0;
}

const EXPORT_API int getButtonIndex(int a_Button) {
	return CommonControllers[joystickType].m_Buttons.m_Buttons[a_Button];
}

const EXPORT_API char* getButtonNameConverted(int a_Button,bool a_IsXbox) {
	return getButtonName(getButtonIndex(a_Button),a_IsXbox);
}

const EXPORT_API char* getDeviceName() {
	if (joystick) {
		return deviceInfo.tszProductName;
	}
	return "";
}

const EXPORT_API GUID getDeviceGUID() {
	if (joystick) {
		return deviceInfo.guidProduct;
	}
	return GUID();
}

const EXPORT_API int getJoystickType() {
	return joystickType;
}

const EXPORT_API char * getKnownDeviceName() {
	return CommonControllers[joystickType].m_Base.m_Name;
}

CALLBACK_FUNC enumJoystickCountCallback(const DIDEVICEINSTANCE * instance, VOID * context) {
	//todo: enumJoystickCountCallback
	return DIENUM_STOP;
}

BOOL CALLBACK enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	for (int i = 1; i < NUM_OF_COMMON_CONTROLLER_TYPES; i++) {
		if (instance->guidProduct == CommonControllers[i].m_Base.m_Guid) {
			joystickType = i;
			break;
		}
	}
	//only allow ps4 controllers
	//if (joystickType != 1) {
	//	return DIENUM_CONTINUE;
	//}

	//select the first joystick
	hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}



	return DIENUM_STOP;
}

BOOL CALLBACK enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context) {
	HRESULT hr;
	HWND hDlg = (HWND)context;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -MAX_AXES_VALUE;
	propRange.lMax = MAX_AXES_VALUE;

	hr = joystick->SetProperty(DIPROP_RANGE, &propRange.diph);
	if (FAILED(hr)) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

float getAxisFromEnum(Axes a_Axis) {
	float value = 0;
	switch (a_Axis) {
	case Axes::LStickX:
		value = joystickState.lX;
		break;
	case Axes::LStickY:
		value = joystickState.lY;
		break;
	case Axes::RStickX:
		value = joystickState.lZ;
		break;
	case Axes::RStickY:
		value = joystickState.lRz;
		break;
	//case Axes::LeftTrigger:
	//	value = (joystickState.lRx + MAX_AXES_VALUE) / 2.0f;
	//	break;
	//case Axes::RightTrigger:
	//	value = (joystickState.lRy + MAX_AXES_VALUE) / 2.0f;
	//	break;
	case Axes::LeftTrigger:
		value = joystickState.lRx;
		break;
	case Axes::RightTrigger:
		value = joystickState.lRy;
		break;
	default:
		return 0.0f;
	}
	return value;
}