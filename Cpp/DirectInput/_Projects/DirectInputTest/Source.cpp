
#include <iostream>

#include "DirectInput.h"
#include "ControllerMapping.h"

int main() {
	HRESULT hr;
	hr = startInput();
	if (FAILED(hr)) {
		return hr;
	}

	std::cout << "Device name: " << getDeviceName() << "\nGUID ";
	GUID guid = getDeviceGUID();
	std::cout <<  guid.Data1 << ", " << guid.Data2 << ", " << guid.Data3 << ", (";
	for (int i = 0; i < 8; i++) {
		if (i != 0) {
			std::cout << ", ";
		}
		std::cout << (int)guid.Data4[i];
	}

	std::cout << ")" << std::endl;

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
		std::cout << "Button numbers: " << std::endl << "\t";
		for (int i = 0; i < getNumOfButtons(); i++) {
			if (getButton(i)) {
				std::wcout << i << "(" << getButtonName(i) << ")" << ", ";
			}
		}
		std::cout << std::endl;
		if (getButton(PS4_START)) {
			loop = false;
		}
	}

	releaseInput();

	return 1;
}