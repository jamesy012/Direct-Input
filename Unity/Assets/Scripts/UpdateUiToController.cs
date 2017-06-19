using System.Collections;
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

	public JInput.Controller m_Controller;


    // Use this for initialization
    void Start () {
		m_Controller = JInput.Input.m_Input.getController(0);
        m_PovImages = m_Pov.GetComponentsInChildren<RawImage>();
        m_PovText = m_Pov.GetComponentInChildren<Text>();
	}
	
	// Update is called once per frame
	void Update () {
        m_LeftTrigger.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.LeftTrigger];
        m_RightTrigger.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.RightTrigger];

        m_LeftStickX.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.LStickX];
        m_LeftStickY.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.LStickY];

        m_RightStickX.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.RStickX];
        m_RightStickY.value = m_Controller.data.axesValues[(int)JInput.ControllerAxes.RStickY];

        int hat = m_Controller.data.hatSwitch;
        m_PovText.text = JInput.Input.GetNameFromHatDir(m_Controller);

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
		for(int i = 0; i < m_Controller.data.buttons.Length; i++) {
			if (m_Controller.data.buttons[i]) {
				if(buttons != 0) {
					buttonsDown += ", ";
				}
				buttons++;
				buttonsDown += JInput.Input.GetNameFromButton(i);
			}
		}
		m_ButtonText.text = buttonsDown;

    }
}
