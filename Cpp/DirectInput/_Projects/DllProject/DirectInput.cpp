
#include "DirectInput.h"
#include "ControllerMapping.h"

#include <vector>

#include "Controller.h"



//members/constants
#define MAX_AXES_VALUE 1000

//list of all controllers
std::vector<UserController*> m_UserControllers;
std::vector<WindowsControllerData*> m_WindowsControllers;
int m_NumOfControllersFound = 0;
int m_NumOfNewControllersAdded = 0;

WindowsControllerData* m_LatestWindowsController = nullptr;

HWND m_WindowHandle = NULL;

//which controller are we currently looking at
int m_CurrentControllerIndex = 0;

//this is the direct input object used through out the class
LPDIRECTINPUT8 di = nullptr;

/*** FUNCTIONS */

/** Non exported functions/DEFINITIONS */
//these are here to prevent them for showing up when the .h is included

//will create a device for all connected joysticks
//currently just creates them for the first device it finds
CALLBACK_FUNC enumJoystickCountCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//will create a device for joysticks that haven't been added yet
CALLBACK_FUNC enumJoystickOldControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//will create a device for all connected joysticks
CALLBACK_FUNC enumJoystickNewControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//will create a device for all connected joysticks
CALLBACK_FUNC enumJoystickControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context);
//sets up the axes for a joystick
CALLBACK_FUNC enumAxesSetCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
//gets the axis value from Axes enum
float getAxisFromEnum(Axes a_Axis);

int setUpControllerBasics();

//returns the current controller
UserController* getCurrentUserController();
//returns the WindowsControllerData for the current controller
WindowsControllerData* getCurrentWCD();

/*** CODE */

const EXPORT_API int startInput() {
	if (di != nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	if (m_WindowHandle == NULL) {
		m_WindowHandle = GetActiveWindow();
	}
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
	for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
		if (m_WindowsControllers[i]->joystick != nullptr) {
			m_WindowsControllers[i]->joystick->Unacquire();
			m_WindowsControllers[i]->joystick->Release();
			delete m_WindowsControllers[i];
			m_WindowsControllers[i] = nullptr;
		}
	}

	for (size_t i = 0; i < m_UserControllers.size(); i++) {
		if (m_UserControllers[i] != nullptr) {
			delete m_UserControllers[i];
			m_UserControllers[i] = nullptr;
		}
	}

	m_WindowsControllers.clear();
	m_UserControllers.clear();

	di->Release();
	di = nullptr;

	m_NumOfControllersFound = 0;
	m_CurrentControllerIndex = 0;
	m_LatestWindowsController = nullptr;
}

const EXPORT_API int updateInput() {

	for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
		updateInputController((int)i);
	}

	return S_OK;
}

const EXPORT_API int updateInputController(int a_Index) {
	HRESULT hr;
	WindowsControllerData* controller = m_WindowsControllers[a_Index];
	if (controller == nullptr) {
		return S_OK;
	}
	if (controller->joystick == NULL) {
		return S_OK;
	}
	if (!controller->acquired) {
		return S_OK;
	}

	hr = controller->joystick->Poll();
	if (FAILED(hr)) {
		//lost connection to the joystick

		int maxAttempts = 50;
		do {
			maxAttempts--;
			hr = controller->joystick->Acquire();
		} while (hr == DIERR_INPUTLOST && maxAttempts >= 0);

		if (maxAttempts <= 0 || hr != DIERR_ACQUIRED) {
			controller->acquired = false;
		}
		if (hr == S_OK) {
			controller->acquired = true;
		}

		if (hr == DIERR_INVALIDPARAM || hr == DIERR_NOTINITIALIZED) {
			return E_FAIL;
		}

		if (hr == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
		//controller.acquired = true;
	}

	hr = controller->joystick->GetDeviceState(sizeof(DIJOYSTATE2), &controller->joystickState);
	if (FAILED(hr)) {
		return hr;
	}
	return S_OK;
}

const EXPORT_API int updateControllers() {
	//acquire controllers if they are not acquired already
	for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
		if (m_WindowsControllers[i] != nullptr) {
			if (!m_WindowsControllers[i]->acquired) {
				m_WindowsControllers[i]->joystick->Acquire();
			}
		}
	}

	//get current number of controllers
	int startNumControllers = m_NumOfControllersFound;

	//count amount of controllers 
	m_NumOfControllersFound = 0;
	di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickCountCallback, NULL, DIEDFL_ATTACHEDONLY);

	//work out amount of controllers taken away or added
	int newControllerCount = m_NumOfControllersFound - startNumControllers;

	//if there was a new controller, then reset all controllers and remake lists
	if (newControllerCount > 0) {

		//reset all controllers
		for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
			if (m_WindowsControllers[i] != nullptr) {
				//if (!m_WindowsControllers[i]->acquired) {
					m_WindowsControllers[i]->joystick->Unacquire();
					m_WindowsControllers[i]->joystick->Release();

					//remove reference to this controller
					m_WindowsControllers[i]->userController->windowsController = nullptr;

					delete m_WindowsControllers[i];
					m_WindowsControllers[i] = nullptr;
				//}
			}
		}

		m_WindowsControllers.clear();

		HRESULT hr;
		////go and set up all old controllers
		//hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickOldControllerCallback, NULL, DIEDFL_ATTACHEDONLY);
		//if (FAILED(hr)) {
		//	return 0;
		//}
		//
		////go through all new controllers
		//hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickNewControllerCallback, NULL, DIEDFL_ATTACHEDONLY);
		//if (FAILED(hr)) {
		//	return 0;
		//}

		//go through all controllers
		//this will only add a new windows controller if one with it's instance id doesnt already exist 
		hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoystickControllerCallback, NULL, DIEDFL_ATTACHEDONLY);
		if (FAILED(hr)) {
			return 0;
		}

		//go through all new controllers
		for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
			WindowsControllerData* wcd = m_WindowsControllers[i];
			//there is a chance this controller has been disconnect
			if (wcd == nullptr) {
				continue;
			}
			//check to see if it matches one of controllers that have been plugged in before
			UserController* controller = nullptr;
			//go through current/past controllers
			for (size_t q = 0; q < m_UserControllers.size(); q++) {
				//check if it has been connected previously 
				if (m_UserControllers[q]->controllerGuid == wcd->deviceInfo.guidInstance) {
					controller = m_UserControllers[q];
					break;
				}
			}
			//this controller has not been connected before
			if (controller == nullptr) {
				//create new controller and assign information for it
				controller = new UserController();
				controller->controllerGuid = wcd->deviceInfo.guidInstance;
				controller->controllerIndex = (int)m_UserControllers.size();

				//get joystick type
				for (int q = 1; q < NUM_OF_COMMON_CONTROLLER_TYPES; q++) {
					if (wcd->deviceInfo.guidProduct == CommonControllers[q].m_Base.m_Guid) {
						controller->joystickType = q;
						break;
					}
				}

				//add to list of controllers
				m_UserControllers.push_back(controller);
			}
			//tell user controller and the windows controller which one 
			controller->windowsController = wcd;
			wcd->userController = controller;

		}
	}

	return newControllerCount;
}

const EXPORT_API void setCurrentController(int a_Index) {
	m_CurrentControllerIndex = a_Index;
}

const EXPORT_API int getNumberOfControllers() {
	return (int)m_UserControllers.size();
}

const EXPORT_API bool isControllerXbox() {
	return getCurrentUserController()->joystickType != 1;
}

const EXPORT_API bool isControllerActive() {
	if (getCurrentWCD() == nullptr) {
		return false;
	}
	return getCurrentWCD()->acquired;
}

const EXPORT_API int getControllerIndex() {
	return getCurrentUserController()->controllerIndex;
}

const EXPORT_API int getButton(int a_Index) {
	if (getCurrentWCD()->joystick) {
		if (a_Index >= (int)getCurrentWCD()->capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		int index = getButtonIndex(a_Index);
		return getCurrentWCD()->joystickState.rgbButtons[index];
	}
	return 0;
}

const EXPORT_API int getButtonNormal(int a_Index) {
	if (getCurrentWCD()->joystick) {
		if (a_Index >= (int)getCurrentWCD()->capabilities.dwButtons || a_Index <= -1) {
			return 0;
		}
		return getCurrentWCD()->joystickState.rgbButtons[a_Index];
	}
	return 0;
}

const EXPORT_API float getAxesValue(int a_Index) {
	if (getCurrentWCD()->joystick) {
		Axes index = CommonControllers[getCurrentUserController()->joystickType].m_Axes.m_Axes[a_Index];
		return getAxisFromEnum(index);
	}
	return 0;
}

const EXPORT_API int getPovValue() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->joystickState.rgdwPOV[0];
	}
	return 0;
}

const EXPORT_API int getPovDir() {
	int hat = 0;
	if (getCurrentWCD()->joystick) {
		hat = ((int)getCurrentWCD()->joystickState.rgdwPOV[0] / 4500) + 1;
		if (getCurrentWCD()->joystickState.rgdwPOV[0] == -1 || (unsigned int)hat >= 10) {
			hat = 0;
		}
	}
	return hat;
}

const EXPORT_API char* getPovName(int a_Dir) {
	if (getCurrentWCD()->joystick) {
		if (a_Dir >= 9 || a_Dir <= -1) {
			a_Dir = 0;
		}
		return hatDirections[a_Dir];
	}
	return hatDirections[0];
}

const EXPORT_API int getNumOfButtons() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->capabilities.dwButtons;
	}
	return 0;
}

const EXPORT_API int getNumOfAxis() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->capabilities.dwAxes;
	}
	return 0;
}

const EXPORT_API int getNumOfPov() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->capabilities.dwPOVs;
	}
	return 0;
}

const EXPORT_API int getButtonIndex(int a_Button) {
	int type = getCurrentUserController()->joystickType;
	Controller_Buttons buttons = CommonControllers[type].m_Buttons;
	//do conversion
	return buttons.m_Buttons[a_Button];
}

const EXPORT_API char* getButtonNameConverted(int a_Button, bool a_IsXbox) {
	return getButtonName(getButtonIndex(a_Button), a_IsXbox);
}

const EXPORT_API char* getDeviceName() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->deviceInfo.tszProductName;
	}
	return "";
}

const EXPORT_API GUID getDeviceGUID() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->deviceInfo.guidProduct;
	}
	return GUID();
}

const EXPORT_API GUID getDeviceInstanceGUID() {
	if (getCurrentWCD()->joystick) {
		return getCurrentWCD()->deviceInfo.guidInstance;
	}
	return GUID();
}

const EXPORT_API int getJoystickType() {
	return getCurrentUserController()->joystickType;
}

const EXPORT_API char * getKnownDeviceName() {
	int controllerType = getCurrentUserController()->joystickType;
	Controller_Data cData = CommonControllers[controllerType];
	return cData.m_Base.m_Name;
}

CALLBACK_FUNC enumJoystickCountCallback(const DIDEVICEINSTANCE * instance, VOID * context) {
	m_NumOfControllersFound++;
	return DIENUM_CONTINUE;
}

BOOL CALLBACK enumJoystickOldControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	UserController* thisController = nullptr;
	// check to see if this device has been added before, and get index if it has
	for (size_t i = 0; i < m_UserControllers.size(); i++) {
		if (m_UserControllers[i]->controllerGuid == instance->guidInstance) {
			thisController = m_UserControllers[i];
			break;
		}
	}

	//this is not a old computer, skip it
	if (thisController == nullptr) {
		return DIENUM_CONTINUE;
	}


	//create WindowsControllerData
	WindowsControllerData* wcd = new WindowsControllerData();
	m_WindowsControllers.push_back(wcd);

	wcd->userController = thisController;
	thisController->windowsController = wcd;

	m_LatestWindowsController = wcd;

	//select the first joystick
	hr = di->CreateDevice(instance->guidInstance, &wcd->joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	if (wcd->joystick == NULL) {
		return DIENUM_CONTINUE;
	}

	hr = setUpControllerBasics();
	if (FAILED(hr)) {
		return hr;
	}

	hr = wcd->joystick->Acquire();
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	wcd->acquired = true;

	return DIENUM_CONTINUE;
}

BOOL CALLBACK enumJoystickNewControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context) {
	HRESULT hr;

	// check to see if this device has been added before, and get index if it has
	for (size_t i = 0; i < m_UserControllers.size(); i++) {
		if (m_UserControllers[i]->controllerGuid == instance->guidInstance) {
			return DIENUM_CONTINUE;
		}
	}
	//create new controller
	UserController* thisController = new UserController();

	//create WindowsControllerData
	WindowsControllerData* wcd = new WindowsControllerData();


	wcd->userController = thisController;
	thisController->windowsController = wcd;

	m_LatestWindowsController = wcd;

	//get joystick type
	for (int i = 1; i < NUM_OF_COMMON_CONTROLLER_TYPES; i++) {
		if (instance->guidProduct == CommonControllers[i].m_Base.m_Guid) {
			thisController->joystickType = i;
			break;
		}
	}

	hr = di->CreateDevice(instance->guidInstance, &wcd->joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	if (wcd->joystick == NULL) {
		return DIENUM_CONTINUE;
	}

	hr = setUpControllerBasics();
	if (FAILED(hr)) {
		return hr;
	}


	thisController->controllerGuid = wcd->deviceInfo.guidInstance;

	thisController->controllerIndex = (int)m_UserControllers.size();
	m_UserControllers.push_back(thisController);
	m_WindowsControllers.push_back(wcd);
	m_NumOfNewControllersAdded++;

	hr = wcd->joystick->Acquire();
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	wcd->acquired = true;

	return DIENUM_CONTINUE;
}


BOOL CALLBACK enumJoystickControllerCallback(const DIDEVICEINSTANCE* instance, VOID* context) {

	//check to see if this instance is currently in the system
	for (size_t i = 0; i < m_WindowsControllers.size(); i++) {
		if (m_WindowsControllers[i] != nullptr) {
			if (m_WindowsControllers[i]->deviceInfo.guidInstance == instance->guidInstance) {
				return DIENUM_CONTINUE;
			}
		}
	}

	HRESULT hr;
	//create WindowsControllerData
	WindowsControllerData* wcd = new WindowsControllerData();
	m_LatestWindowsController = wcd;//latest controller required for setting up axes

	hr = di->CreateDevice(instance->guidInstance, &wcd->joystick, NULL);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	if (wcd->joystick == NULL) {
		return DIENUM_CONTINUE;
	}

	hr = setUpControllerBasics();
	if (FAILED(hr)) {
		return hr;
	}

	m_WindowsControllers.push_back(wcd);

	hr = wcd->joystick->Acquire();
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	wcd->acquired = true;

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

	hr = m_LatestWindowsController->joystick->SetProperty(DIPROP_RANGE, &propRange.diph);
	if (FAILED(hr)) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

float getAxisFromEnum(Axes a_Axis) {
	LONG value = 0;
	WindowsControllerData* wcd = getCurrentWCD();
	switch (a_Axis) {
	case Axes::LStickX:
		value = wcd->joystickState.lX;
		break;
	case Axes::LStickY:
		value = wcd->joystickState.lY;
		break;
	case Axes::RStickX:
		value = wcd->joystickState.lZ;
		break;
	case Axes::RStickY:
		value = wcd->joystickState.lRz;
		break;
		//case Axes::LeftTrigger:
		//	value = (m_Controllers[m_ControllerIndex]->joystickState.lRx + MAX_AXES_VALUE) / 2.0f;
		//	break;
		//case Axes::RightTrigger:
		//	value = (m_Controllers[m_ControllerIndex]->joystickState.lRy + MAX_AXES_VALUE) / 2.0f;
		//	break;
	case Axes::LeftTrigger:
		value = wcd->joystickState.lRx;
		break;
	case Axes::RightTrigger:
		value = wcd->joystickState.lRy;
		break;
	default:
		return 0.0f;
	}
	return (float)value;
}

int setUpControllerBasics() {
	HRESULT hr;
	//tell it which format to use
	hr = m_LatestWindowsController->joystick->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) {
		return hr;
	}

	//set how this controller is acquired in relation to other programs and this
	hr = m_LatestWindowsController->joystick->SetCooperativeLevel(m_WindowHandle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hr)) {
		return hr;
	}

	//go through each axes on the controller and set it's
	hr = m_LatestWindowsController->joystick->EnumObjects(enumAxesSetCallback, NULL, DIDFT_AXIS);
	if (FAILED(hr)) {
		return hr;
	}

	//set up struct sizes
	m_LatestWindowsController->capabilities.dwSize = sizeof(DIDEVCAPS);
	m_LatestWindowsController->deviceInfo.dwSize = sizeof(DIDEVICEINSTANCEA);

	//get capability's of joystick
	hr = m_LatestWindowsController->joystick->GetCapabilities(&m_LatestWindowsController->capabilities);
	if (FAILED(hr)) {
		return hr;
	}

	//get info of joystick
	hr = m_LatestWindowsController->joystick->GetDeviceInfo(&m_LatestWindowsController->deviceInfo);
	if (FAILED(hr)) {
		return hr;
	}
	return S_OK;
}

UserController * getCurrentUserController() {
	return m_UserControllers[m_CurrentControllerIndex];
}

//get the current user Controller, then gets it windowsController
//if m_UserControllres is empty, or the current controller index points to a empty slot this will cause a error
WindowsControllerData * getCurrentWCD() {
	return getCurrentUserController()->windowsController;
}
