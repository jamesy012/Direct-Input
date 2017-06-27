#pragma once

#include "Controller.h"

#define NUM_OF_COMMON_CONTROLLER_TYPES 5
//if two controllers use the same mapping then there will be duplicated data :/
//might change is so they reference their buttons and axis instead of having the same data for each controller
//Note: for the same controller the GUID was different between computers. might have to change to a name/string compare

//this represents the controller name/guid
//and which button corresponds to a standardized mapping
const Controller_Data CommonControllers[NUM_OF_COMMON_CONTROLLER_TYPES] = {
	Controller_Data{//this is the default controller
	/*guid*/	Controller_Info{ "UNKNOWN",GUID{ 0,0,0,{ 0,0,0,0,0,0,0,0 } } },
	/*buttons*/ Controller_Buttons{ { 0,1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,32,33,34,35 } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::RStickX,Axes::RStickY,Axes::LeftTrigger },TriggerType::Two }
},
Controller_Data{
	/*guid*/	Controller_Info{ "PS4 Controller",GUID{ 96732492,0,0,{ 0,0,80,73,68,86,73,68 } } },
	/*buttons*/ Controller_Buttons{ { 1, 2, 0, 3, 4, 5,10, 11, 8, 9,12, 6, 7,13, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::RStickX,Axes::RStickY,Axes::LeftTrigger,Axes::RightTrigger },TriggerType::Two }
},
Controller_Data{
	/*guid*/	Controller_Info{ "Game pad F310", GUID{ 3256681581,0,0,{ 0, 0, 80, 73, 68, 86, 73, 68 } } },
	/*buttons*/ Controller_Buttons{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::LeftTrigger,Axes::RightTrigger,Axes::LStickX },TriggerType::One }
},
Controller_Data{
	/*guid*/	Controller_Info{ "Xbox one controller", GUID{ 47252574,0,0,{ 0, 0, 80, 73, 68, 86, 73, 68 } } },
	/*buttons*/ Controller_Buttons{ { 0, 1, 2, 3, 4, 5, 8, 9, 6, 7, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::LeftTrigger,Axes::RightTrigger,Axes::RStickX,Axes::RStickX },TriggerType::One }
},
Controller_Data{
	/*guid*/	Controller_Info{ "Xbox one controller", GUID{ 50267230,0,0,{ 0, 0, 80, 73, 68, 86, 73, 68 } } },
	/*buttons*/ Controller_Buttons{ { 0, 1, 2, 3, 4, 5, 8, 9, 6, 7, } },
	/*Axes*/	Controller_Axis{ { Axes::LStickX,Axes::LStickY,Axes::LeftTrigger,Axes::RightTrigger,Axes::RStickX,Axes::RStickX },TriggerType::One }
},
};