﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UpdateUiToController : MonoBehaviour {

    public Slider m_LeftTrigger;
    public Slider m_RightTrigger;

    public Slider m_LeftStickX;
    public Slider m_LeftStickY;

    public Slider m_RightStickX;
    public Slider m_RightStickY;

    public GameObject m_Pov;
    private RawImage[] m_PovImages = new RawImage[4];
    private Text m_PovText;

	public Text m_ButtonText;

    //public JInput.Controller m_Controller;
    [Range(0,16)]
    public int m_ControllerIndex;

    // Use this for initialization
    void Start () {
		//m_Controller = JInput.Input.m_Input.getController(0);
        m_PovImages = m_Pov.GetComponentsInChildren<RawImage>();
        m_PovText = m_Pov.GetComponentInChildren<Text>();
	}
	
	// Update is called once per frame
	void Update () {
        JInput.Controller controller = JInput.InputManager.getController(m_ControllerIndex);
        if(controller == null) {
            return;
        }
        m_LeftTrigger.value = controller.data.axesValues[(int)JInput.ControllerAxes.LeftTrigger];
        m_RightTrigger.value = controller.data.axesValues[(int)JInput.ControllerAxes.RightTrigger];

        m_LeftStickX.value = controller.data.axesValues[(int)JInput.ControllerAxes.LStickX];
        m_LeftStickY.value = controller.data.axesValues[(int)JInput.ControllerAxes.LStickY];

        m_RightStickX.value = controller.data.axesValues[(int)JInput.ControllerAxes.RStickX];
        m_RightStickY.value = controller.data.axesValues[(int)JInput.ControllerAxes.RStickY];

        int hat = controller.data.hatSwitch;
        m_PovText.text = JInput.InputManager.GetNameFromHatDir(controller);

        if (hat == 1 || hat == 8 || hat == 2) {
            m_PovImages[0].color = Color.red;
        }else {
            m_PovImages[0].color = Color.white;
        }

        if (hat == 2 || hat == 3 || hat == 4) {
            m_PovImages[1].color = Color.red;
        } else {
            m_PovImages[1].color = Color.white;
        }

        if (hat == 4 || hat == 5 || hat == 6) {
            m_PovImages[2].color = Color.red;
        } else {
            m_PovImages[2].color = Color.white;
        }

        if (hat == 6 || hat == 7 || hat == 8) {
            m_PovImages[3].color = Color.red;
        } else {
            m_PovImages[3].color = Color.white;
        }

		string buttonsDown = "";
		int buttons = 0;
		for(int i = 0; i < controller.data.buttons.Length; i++) {
			if (controller.data.buttons[i]) {
				if(buttons != 0) {
					buttonsDown += ", ";
				}
				buttons++;
				buttonsDown += JInput.InputManager.GetNameFromButton(controller, i);
			}
		}
		m_ButtonText.text = buttonsDown;

    }
}
