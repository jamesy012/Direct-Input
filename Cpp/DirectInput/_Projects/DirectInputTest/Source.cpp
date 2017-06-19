
#include <iostream>

#include "DirectInput.h"
#include "ControllerMapping.h"

int main() {
	HRESULT hr;
	hr = startInput();
	if (FAILED(hr)) {
		return hr;
	}
	int counter = 0;
	bool loop = true;
	while (loop) {
		counter++;
		if (counter % 10000000 != 0) {
			continue;
		}
		system("cls");
		updateInput();
		
		std::cout << getAxesValue(Axes::LeftTrigger) << std::endl;

		for (int i = 0; i < getNumOfButtons(); i++) {
			if (getButton(i)) {
				std::cout << i << ", ";
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