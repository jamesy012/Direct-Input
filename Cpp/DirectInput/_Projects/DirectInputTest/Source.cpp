
#include <iostream>

#include "DirectInputHeader.h"


void main() {

	startInput();


	bool loop = true;
	while (loop) {
		updateInput();

		std::cout << getAxesValue(Axes::LeftTrigger) << std::endl;

		if (getButton(0)) {
			loop = false;
		}
	}

	releaseInput();

}