
#include <iostream>
#include <string>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include "DirectInputKeyNames.h"


LPDIRECTINPUT8 di;
LPDIRECTINPUTDEVICE8 joystick;
LPDIRECTINPUTDEVICE8 keyboard;
unsigned char keyboardState[256];


typedef struct MYDATA {
	union {
		struct {
			LONG  lX;                   // X-axis goes here. 
			LONG  lY;                   // Y-axis goes here. 
			BYTE  bButtonA;             // One button goes here. 
			BYTE  bButtonB;             // Another button goes here. 
			BYTE  bPadding[2];          // Must be DWORD multiple in size. 
		};
		BYTE data[6];
	};
} MYDATA;

// Then, it can use the following data format. 

DIOBJECTDATAFORMAT rgodf[] = {
	{ &GUID_XAxis, FIELD_OFFSET(MYDATA, lX),
	DIDFT_AXIS | DIDFT_ANYINSTANCE, 0, },
	{ &GUID_YAxis, FIELD_OFFSET(MYDATA, lY),
	DIDFT_AXIS | DIDFT_ANYINSTANCE, 0, },
	{ &GUID_Button, FIELD_OFFSET(MYDATA, bButtonA),
	DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
	{ &GUID_Button, FIELD_OFFSET(MYDATA, bButtonB),
	DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0, },
};

#define numObjects (sizeof(rgodf) / sizeof(rgodf[0])) 

DIDATAFORMAT df = {
	sizeof(DIDATAFORMAT),       // Size of this structure 
	sizeof(DIOBJECTDATAFORMAT), // Size of object data format 
	DIDF_ABSAXIS,               // Absolute axis coordinates 
	sizeof(MYDATA),             // Size of device data 
	numObjects,                 // Number of objects 
	rgodf,                      // And here they are 
};

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}
	return DIENUM_CONTINUE;
	//return DIENUM_STOP;
}

BOOL CALLBACK enumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, void* context) {
	HRESULT hr;
	HWND hDlg = (HWND) context;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -1000;
	propRange.lMax = 1000;

	hr = joystick->SetProperty(DIPROP_RANGE, &propRange.diph);
	if (FAILED(hr)) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;

}

HRESULT pollJoystick(DIJOYSTATE2* js) {
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

	MYDATA test1;

	hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js);
	//hr = joystick->GetDeviceState(sizeof(MYDATA), &test1);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

HRESULT pollKeyboard() {
	HRESULT hr;

	if (keyboard == NULL) {
		return S_OK;
	}

	hr = keyboard->Poll();
	hr = keyboard->GetDeviceState(sizeof(keyboardState), (void*) keyboardState);
	//hr = joystick->Poll();
	if (FAILED(hr)) {

		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			hr = keyboard->Acquire();
		}
	}

	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

int main() {
	HWND handle = GetConsoleWindow();

	HRESULT hr;

	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**) &di, NULL);
	if (FAILED(hr)) {
		return hr;
	}

	//keyboard
	hr = di->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if (FAILED(hr)) {
		return hr;
	}
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) {
		return hr;
	}
	hr = keyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		return hr;
	}
	hr = keyboard->Acquire();
	if (FAILED(hr)) {
		return hr;
	}

	//joystick/controller
	hr = di->EnumDevices(DI8DEVCLASS_ALL, enumCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		return hr;
	}

	if (joystick == NULL) {
		printf("Joystick not found.\n");
		return E_FAIL;
	}


	hr = joystick->SetDataFormat(&c_dfDIJoystick2);
	//hr = joystick->SetDataFormat(&df);
	if (FAILED(hr)) {
		return hr;
	}

	hr = joystick->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hr)) {
		return hr;
	}

	DIDEVCAPS capabilities;
	capabilities.dwSize = sizeof(DIDEVCAPS);
	hr = joystick->GetCapabilities(&capabilities);
	if (FAILED(hr)) {
		return hr;
	}

	hr = joystick->EnumObjects(enumAxesCallback, NULL, DIDFT_AXIS);
	if (FAILED(hr)) {
		return hr;
	}

	hr = joystick->Acquire();


	DIDEVICEINSTANCE deviceInfo;
	deviceInfo.dwSize = sizeof(DIDEVICEINSTANCE);
	hr = joystick->GetDeviceInfo(&deviceInfo);

	const int VALUES = 20;
	DIDEVICEOBJECTDATA rgdod[VALUES];
	DWORD dwItems = VALUES;
	hr = joystick->GetDeviceData(
		sizeof(DIDEVICEOBJECTDATA),
		rgdod,
		&dwItems,
		0);

	DIDEVICEOBJECTINSTANCE objectInfo;
	objectInfo.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
	hr = joystick->GetObjectInfo(&objectInfo, 0,DIPH_DEVICE);

	DIPROPHEADER propTest;
	propTest.dwSize = sizeof(DIPROPHEADER);
	propTest.dwHeaderSize = sizeof(DIPROPHEADER);
	propTest.dwObj = 0;
	propTest.dwHow = DIPH_DEVICE;

	hr = joystick->GetProperty(GUID_RxAxis,&propTest);

	//polling/printing to screen
	DIJOYSTATE2 joystickState;

	std::string currWord = "";

	int counter = 0;
	while (true) {
		if (counter % 10000000 == 0) {
			pollJoystick(&joystickState);
			pollKeyboard();

			system("cls");
			std::cout << "Left Stick:\tx:" << joystickState.lX << "\ty:" << joystickState.lY << std::endl;
			std::cout << "Right Stick:\tx:" << joystickState.lZ << "\ty:" << joystickState.lRz << std::endl;
			std::cout << "Left Trigger:\t" << (joystickState.lRx + 1000) / 2.0f << std::endl;
			std::cout << "Right Trigger:\t" << (joystickState.lRy + 1000) / 2.0f << std::endl;

			std::cout << "Buttons Down:" << std::endl << "\t";
			for (int i = 0; i < capabilities.dwButtons; i++) {
				if (joystickState.rgbButtons[i]) {
					std::cout << i << ", ";
				}
			}
			std::cout << std::endl;

			int hat = ((int) joystickState.rgdwPOV[0] / 4500) + 1;
			if (joystickState.rgdwPOV[0] == -1 || (unsigned int) hat >= 10) {
				hat = 0;
			}

			std::cout << "hat/pov Dir:" << hat << "(" << (int) joystickState.rgdwPOV[0] << ")" << "\t";
			const char* names[] = { "None","Up","Up-Right","Right","Down-Right","Down","Down-Left","Left","Up-Left" };
			std::cout << names[hat] << std::endl;

			std::cout << std::endl;

			for (int i = 0; i < 256; i++) {
				if (int(keyboardState[i])) {
					std::cout << keys[i] << "(" << i << ")" << ", ";

					if (i == DIK_RETURN) {
						currWord = "";
					} else {
						currWord += keys[i];
					}
				}
			}
			std::cout << std::endl;

			std::cout << currWord << std::endl;
		}
		counter++;
	}

	if (joystick) {
		joystick->Unacquire();
	}
	if (keyboard) {
		keyboard->Unacquire();
	}

	system("pause");

	return 1;
}

