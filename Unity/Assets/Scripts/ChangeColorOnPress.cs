using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChangeColorOnPress : MonoBehaviour {

	public JInput.ControllerButtons m_Button;
	public Material m_Mat;

	// Update is called once per frame
	void Update() {
		//loop through controllers
		for (int i = 0; i < JInput.InputManager.numberOfControllers; i++) {
			//check to see if it's pressed
			if (JInput.InputManager.getController(i).WasButtonPressed(m_Button)) {
				//apply random color, and leave for loop
				m_Mat.color = Random.ColorHSV();
				break;
			}
		}

		/*
		//only checking one controller 
		if (JInput.InputManager.getController(0).WasButtonPressed(m_Button)) {
			//apply random color, and leave for loop
			m_Mat.color = Random.ColorHSV();
		}
		*/

	}
}
