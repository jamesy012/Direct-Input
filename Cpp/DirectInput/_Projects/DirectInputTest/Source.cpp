
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

	bool isXbox = getJoystickType() != 1;

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

		std::cout << "Left Trigger: " << getAxesValue(Axes::LeftTrigger) << std::endl;
		std::cout << "Right Trigger: " << getAxesValue(Axes::RightTrigger) << std::endl;
		std::cout << "Left Stick X: " << getAxesValue(Axes::LStickX) << std::endl;
		std::cout << "Left Stick Y: " << getAxesValue(Axes::LStickY) << std::endl;
		std::cout << "Right Stick X: " << getAxesValue(Axes::RStickX) << std::endl;
		std::cout << "Right Stick Y: " << getAxesValue(Axes::RStickY) << std::endl;

		std::cout << "Button numbers: (Converted/Normal)" << std::endl << "\t";
		for (int i = 0; i < getNumOfButtons(); i++) {
			if (getButton(i)) {
				std::wcout << getButtonIndex(i) << "/" << i << "(";
				std::wcout << getName(getButtonIndex(i), isXbox) << "/" << getName(i, isXbox) << ")" << ", ";
			}
		}


		std::cout << std::endl;

		std::wcout << "Pov Dir: " << getPovName(getPovDir());


		if (getButton(Buttons::Start)) {
			loop = false;
		}
	}

	releaseInput();

	return 1;
}