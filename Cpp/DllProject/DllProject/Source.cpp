


#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif


// ------------------------------------------------------------------------
// Plugin itself

#include <dinput.h>

#define MAX_AXES_VALUE 1000

LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joystick;
DIJOYSTATE2 joystickState;
DIDEVCAPS capabilities;

const wchar_t* hatDirections[] = { L"None",L"Up",L"Up-Right",L"Right",L"Down-Right",L"Down",L"Down-Left",L"Left",L"Up-Left" };

enum Axes {
	LStickX,
	LStickY,
	RStickX,
	RStickY,
	LeftTrigger,
	RightTrigger
};

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
	case Axes::LeftTrigger:
		value = (joystickState.lRx + MAX_AXES_VALUE) / 2.0f;
		break;
	case Axes::RightTrigger:
		value = (joystickState.lRy + MAX_AXES_VALUE) / 2.0f;
		break;
	default:
		return 0.0f;
	}
}


// Link following functions C-style (required for plugins)
extern "C"
{


	int EXPORT_API startInput() {
		HRESULT hr;
		HWND handle = GetActiveWindow();

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

		return 1000;
		//return S_OK;
	}

	void EXPORT_API releaseInput() {
		if (joystick) {
			joystick->Unacquire();
			joystick->Release();
		}
		di->Release();
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

	int EXPORT_API getButton(int index) {
		if (joystick) {
			if (index >= capabilities.dwButtons || index <= -1) {
				return 0;
			}
			return joystickState.rgbButtons[index];
		}
		return 0;
	}

	int EXPORT_API getAxesValue(int index) {
		if (joystick) {
			return getAxisFromEnum((Axes)index);
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


} // end of export C block