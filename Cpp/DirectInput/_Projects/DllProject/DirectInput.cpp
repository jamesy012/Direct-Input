
#include "DirectInput.h"
#include "ControllerMapping.h"


const Controller_Data CommonControllers[] = {
	{ "PS4 Controller",GUID{ 96732492,0,0,{0,0,80,73,68,86,73,68 }} }
};


int EXPORT_API startInput() {
	if (di != nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	HWND handle = GetActiveWindow();
	if (handle == NULL) {
		handle = GetConsoleWindow();
	}

	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**) &di, NULL);
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


	DIDEVICEINSTANCEA deviceInfo;
	deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEA);
	hr = joystick->GetDeviceInfo(&deviceInfo);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

void EXPORT_API releaseInput() {
	if (joystick) {
		joystick->Unacquire();
		joystick->Release();
		joystick = nullptr;
	}
	di->Release();
	di = nullptr;
}

int EXPORT_API updateInput() {
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

int EXPORT_API getButton(int a_Index) {
	if (joystick) {
		if (a_Index >= capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		return joystickState.rgbButtons[a_Index];
	}
	return 0;
}

int EXPORT_API getAxesValue(int a_Index) {
	if (joystick) {
		return getAxisFromEnum((Axes) a_Index);
	}
	return 0;
}

int EXPORT_API getPovValue() {
	if (joystick) {
		return joystickState.rgdwPOV[0];
	}
	return 0;
}

int EXPORT_API getPovDir() {
	int hat = 0;
	if (joystick) {
		hat = ((int) joystickState.rgdwPOV[0] / 4500) + 1;
		if (joystickState.rgdwPOV[0] == -1 || (unsigned int) hat >= 10) {
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

int EXPORT_API getNumOfButtons() {
	if (joystick) {
		return capabilities.dwButtons;
	}
	return 0;
}

int EXPORT_API getNumOfAxis() {
	if (joystick) {
		return capabilities.dwAxes;
	}
	return 0;
}

int EXPORT_API getNumOfPov() {
	if (joystick) {
		return capabilities.dwPOVs;
	}
	return 0;
}

const EXPORT_API wchar_t * getButtonName(int a_Button) {
	switch (a_Button) {
		case PS4_CROSS:
			return L"Cross";
		case PS4_CIRCLE:
			return L"Circle";
		case PS4_TRIANGLE:
			return L"Triangle";
		case PS4_SQUARE:
			return L"Square";
		case PS4_L1:
			return L"L1";
		case PS4_R1:
			return L"R1";
		case PS4_L2:
			return L"L2";
		case PS4_R2:
			return L"R2";
		case PS4_SELECT:
			return L"Select";
		case PS4_START:
			return L"Start";
		case PS4_L3:
			return L"L3";
		case PS4_R3:
			return L"R3";
		case PS4_PS:
			return L"PS";
		case PS4_TOUCH_PAD:
			return L"Touch Pad";
		default:
			break;
	}
	return L"UNKNOWN";
}

BOOL CALLBACK enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	//select the first joystick
	hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

BOOL CALLBACK enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context) {
	HRESULT hr;
	HWND hDlg = (HWND) context;

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
		case Axes::LeftTrigger:
			value = (joystickState.lRx + MAX_AXES_VALUE) / 2.0f;
			break;
		case Axes::RightTrigger:
			value = (joystickState.lRy + MAX_AXES_VALUE) / 2.0f;
			break;
		default:
			return 0.0f;
	}
	return value;
}