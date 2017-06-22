
#include <iostream>

#include "DirectInput.h"
#include "Controller.h"
#include "ControllerMapping.h"

int main() {
	HRESULT hr;
	hr = startInput();
	if (FAILED(hr)) {
		return hr;
	}
	for (int controller = 0; controller < getNumberOfControllers(); controller++) {
		setCurrentController(controller);
		std::cout << "Device name: " << getDeviceName() << " Known name: " << getKnownDeviceName() << "\nGUID ";
		GUID guid = getDeviceGUID();
		std::cout << guid.Data1 << ", " << guid.Data2 << ", " << guid.Data3 << ", (";
		for (int i = 0; i < 8; i++) {
			if (i != 0) {
				std::cout << ", ";
			}
			std::cout << (int)guid.Data4[i];
		}

		std::cout << ")" << std::endl;

		std::cout << "Number of buttons: " << getNumOfButtons() << std::endl;

		std::cout << "Controller is " << (isControllerXbox() ? "Xbox" : "Ps4") << std::endl;

		std::cout << std::endl;
	}


	system("pause");

	int counter = 0;
	bool loop = true;
	while (loop) {
		counter++;
		if (counter % 10000000 != 0) {
			continue;
		}
		system("cls");
		updateInput();
		for (int controller = 0; controller < getNumberOfControllers(); controller++) {
			setCurrentController(controller);
			bool isXbox = isControllerXbox();
			std::cout << "Left Trigger: " << getAxesValue(Axes::LeftTrigger) << std::endl;
			std::cout << "Right Trigger: " << getAxesValue(Axes::RightTrigger) << std::endl;
			std::cout << "Left Stick X: " << getAxesValue(Axes::LStickX) << std::endl;
			std::cout << "Left Stick Y: " << getAxesValue(Axes::LStickY) << std::endl;
			std::cout << "Right Stick X: " << getAxesValue(Axes::RStickX) << std::endl;
			std::cout << "Right Stick Y: " << getAxesValue(Axes::RStickY) << std::endl;

			std::cout << "Button numbers: (Converted/Normal)" << std::endl << "\t";
			for (int i = 0; i < getNumOfButtons(); i++) {
				if (getButton(i)) {
					std::cout << getButtonIndex(i) << "/" << i << "(";
					std::cout << getButtonNameConverted(i, isXbox) << "/";
					std::cout << getButtonName(i, isXbox) << ")" << ", ";
				}
			}

			std::cout << std::endl;

			std::cout << "Pov Dir: " << getPovName(getPovDir());

			std::cout << std::endl;
			std::cout << std::endl;

			if (getButton(Buttons::Start)) {
				loop = false;
			}
		}

		int amountOfButtonsDown = 0;
		for (int controller = 0; controller < getNumberOfControllers(); controller++) {
			setCurrentController(controller);
			if (getButton(Buttons::A)) {
				amountOfButtonsDown++;
			}
		}
		if (amountOfButtonsDown == 2) {
			loop &= false;
		}
	}

	releaseInput();

	return 1;
}