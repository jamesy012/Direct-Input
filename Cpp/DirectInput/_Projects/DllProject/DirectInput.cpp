
#include "DirectInput.h"
#include "ControllerMapping.h"



//members/constants
#define MAX_AXES_VALUE 1000

//this is the direct input object used through out the class
LPDIRECTINPUT8 di = nullptr;


const EXPORT_API int startInput() {
	if (di != nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	m_WindowHandle = GetActiveWindow();
	if (m_WindowHandle == NULL) {
		m_WindowHandle = GetConsoleWindow();
	}
	if (m_WindowHandle == NULL) {
		return E_FAIL;
	}

	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&di, NULL);
	if (FAILED(hr)) {
		return hr;
	}

	//hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickSelectCallback, NULL, DIEDFL_ATTACHEDONLY);
	//if (FAILED(hr)) {
	//	return hr;
	//}

	return S_OK;
}

const EXPORT_API void releaseInput() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		m_Controllers[m_ControllerIndex].joystick->Unacquire();
		m_Controllers[m_ControllerIndex].joystick->Release();
		m_Controllers[m_ControllerIndex].joystick = nullptr;
	}
	di->Release();
	di = nullptr;
}

const EXPORT_API int updateInput() {
	
	for (int i = 0; i < m_Controllers.size(); i++) {
		updateInputController(i);
	}

	return S_OK;
}

const EXPORT_API int updateInputController(int a_Index) {
	HRESULT hr;
	Controller& controller = m_Controllers[a_Index];
	if (controller.joystick == NULL) {
		return S_OK;
	}
	if (!controller.acquired) {
		return S_OK;
	}

	hr = controller.joystick->Poll();
	if (FAILED(hr)) {
		//lost connection to the joystick

		//todo: limit on amount of loops, if it fails then remove it from the active list
		int maxAttempts = 50;
		do {
			maxAttempts--;
			hr = controller.joystick->Acquire();
		} while (hr == DIERR_INPUTLOST && maxAttempts >= 0);

		if (maxAttempts <= 0 || hr != DIERR_ACQUIRED) {
			controller.acquired = false;
		}

		if (hr == DIERR_INVALIDPARAM || hr == DIERR_NOTINITIALIZED) {
			return E_FAIL;
		}

		if (hr == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}

	}

	hr = controller.joystick->GetDeviceState(sizeof(DIJOYSTATE2), &controller.joystickState);
	if (FAILED(hr)) {
		return hr;
	}
}

const EXPORT_API int updateControllers() {
	int startNumControllers = m_NumOfControllersFound;

	//go through all controlers finding new ones
	HRESULT hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickSelectCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		return 0;
	}

	return m_NumOfControllersFound - startNumControllers;
}

const EXPORT_API void setCurrentController(int a_Index) {
	m_ControllerIndex = a_Index;
}

const EXPORT_API int getNumberOfControllers() {
	return m_Controllers.size();
}

const EXPORT_API bool isControllerXbox() {
	return m_Controllers[m_ControllerIndex].joystickType != 1;
}

const EXPORT_API bool isControllerActive() {
	return m_Controllers[m_ControllerIndex].acquired;
}

const EXPORT_API int getButton(int a_Index) {
	if (m_Controllers[m_ControllerIndex].joystick) {
		if (a_Index >= m_Controllers[m_ControllerIndex].capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		int index = getButtonIndex(a_Index);
		return m_Controllers[m_ControllerIndex].joystickState.rgbButtons[index];
	}
	return 0;
}

const EXPORT_API int getButtonNormal(int a_Index) {
	if (m_Controllers[m_ControllerIndex].joystick) {
		if (a_Index >= m_Controllers[m_ControllerIndex].capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		return m_Controllers[m_ControllerIndex].joystickState.rgbButtons[a_Index];
	}
	return 0;
}

const EXPORT_API int getAxesValue(int a_Index) {
	if (m_Controllers[m_ControllerIndex].joystick) {
		Axes index = CommonControllers[m_Controllers[m_ControllerIndex].joystickType].m_Axes.m_Axes[a_Index];
		return getAxisFromEnum(index);
	}
	return 0;
}

const EXPORT_API int getPovValue() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].joystickState.rgdwPOV[0];
	}
	return 0;
}

const EXPORT_API int getPovDir() {
	int hat = 0;
	if (m_Controllers[m_ControllerIndex].joystick) {
		hat = ((int)m_Controllers[m_ControllerIndex].joystickState.rgdwPOV[0] / 4500) + 1;
		if (m_Controllers[m_ControllerIndex].joystickState.rgdwPOV[0] == -1 || (unsigned int)hat >= 10) {
			hat = 0;
		}
	}
	return hat;
}

const EXPORT_API char* getPovName(int a_Dir) {
	if (m_Controllers[m_ControllerIndex].joystick) {
		if (a_Dir >= 9 || a_Dir <= -1) {
			a_Dir = 0;
		}
		return hatDirections[a_Dir];
	}
	return hatDirections[0];
}

const EXPORT_API int getNumOfButtons() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].capabilities.dwButtons;
	}
	return 0;
}

const EXPORT_API int getNumOfAxis() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].capabilities.dwAxes;
	}
	return 0;
}

const EXPORT_API int getNumOfPov() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].capabilities.dwPOVs;
	}
	return 0;
}

const EXPORT_API int getButtonIndex(int a_Button) {
	return CommonControllers[m_Controllers[m_ControllerIndex].joystickType].m_Buttons.m_Buttons[a_Button];
}

const EXPORT_API char* getButtonNameConverted(int a_Button,bool a_IsXbox) {
	return getButtonName(getButtonIndex(a_Button),a_IsXbox);
}

const EXPORT_API char* getDeviceName() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].deviceInfo.tszProductName;
	}
	return "";
}

const EXPORT_API GUID getDeviceGUID() {
	if (m_Controllers[m_ControllerIndex].joystick) {
		return m_Controllers[m_ControllerIndex].deviceInfo.guidProduct;
	}
	return GUID();
}

const EXPORT_API int getJoystickType() {
	return m_Controllers[m_ControllerIndex].joystickType;
}

const EXPORT_API char * getKnownDeviceName() {
	return CommonControllers[m_Controllers[m_ControllerIndex].joystickType].m_Base.m_Name;
}

CALLBACK_FUNC enumJoystickCountCallback(const DIDEVICEINSTANCE * instance, VOID * context) {
	//todo: enumJoystickCountCallback
	return DIENUM_STOP;
}

BOOL CALLBACK enumJoystickSelectCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	// check to see if this device has been added before
	for (auto& iter : m_Controllers) {
		if (iter.second.deviceInfo.guidInstance == instance->guidInstance) {
			iter.second.acquired = true;
			return DIENUM_CONTINUE;
		}
	}

	Controller thisController;
	for (int i = 1; i < NUM_OF_COMMON_CONTROLLER_TYPES; i++) {
		if (instance->guidProduct == CommonControllers[i].m_Base.m_Guid) {
			thisController.joystickType = i;
			break;
		}
	}
	//only allow ps4 controllers
	//if (joystickType != 1) {
	//	return DIENUM_CONTINUE;
	//}

	//select the first joystick
	hr = di->CreateDevice(instance->guidInstance, &thisController.joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	if (thisController.joystick == NULL) {
		return DIENUM_CONTINUE;
	}

	m_LatestController = &thisController;
	thisController.acquired = true;

	thisController.capabilities.dwSize = sizeof(DIDEVCAPS);
	thisController.deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEA);

	hr = thisController.joystick->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) {
		return hr;
	}


	hr = thisController.joystick->SetCooperativeLevel(m_WindowHandle, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		return hr;
	}

	hr = thisController.joystick->GetCapabilities(&thisController.capabilities);
	if (FAILED(hr)) {
		return hr;
	}

	hr = thisController.joystick->EnumObjects(enumAxesSetCallback, NULL, DIDFT_AXIS);
	if (FAILED(hr)) {
		return hr;
	}

	hr = thisController.joystick->GetDeviceInfo(&thisController.deviceInfo);
	if (FAILED(hr)) {
		return hr;
	}

	m_Controllers[m_NumOfControllersFound] = thisController;
	m_NumOfControllersFound++;
	return DIENUM_CONTINUE;
}

BOOL CALLBACK enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context) {
	HRESULT hr = S_FALSE;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -MAX_AXES_VALUE;
	propRange.lMax = MAX_AXES_VALUE;

	hr = m_LatestController->joystick->SetProperty(DIPROP_RANGE, &propRange.diph);
	if (FAILED(hr)) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

float getAxisFromEnum(Axes a_Axis) {
	float value = 0;
	switch (a_Axis) {
	case Axes::LStickX:
		value = m_Controllers[m_ControllerIndex].joystickState.lX;
		break;
	case Axes::LStickY:
		value = m_Controllers[m_ControllerIndex].joystickState.lY;
		break;
	case Axes::RStickX:
		value = m_Controllers[m_ControllerIndex].joystickState.lZ;
		break;
	case Axes::RStickY:
		value = m_Controllers[m_ControllerIndex].joystickState.lRz;
		break;
	//case Axes::LeftTrigger:
	//	value = (m_Controllers[m_ControllerIndex].joystickState.lRx + MAX_AXES_VALUE) / 2.0f;
	//	break;
	//case Axes::RightTrigger:
	//	value = (m_Controllers[m_ControllerIndex].joystickState.lRy + MAX_AXES_VALUE) / 2.0f;
	//	break;
	case Axes::LeftTrigger:
		value = m_Controllers[m_ControllerIndex].joystickState.lRx;
		break;
	case Axes::RightTrigger:
		value = m_Controllers[m_ControllerIndex].joystickState.lRy;
		break;
	default:
		return 0.0f;
	}
	return value;
}