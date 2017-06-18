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

    public test m_Test;


    // Use this for initialization
    void Start () {
        m_PovImages = m_Pov.GetComponentsInChildren<RawImage>();
        m_PovText = m_Pov.GetComponentInChildren<Text>();
	}
	
	// Update is called once per frame
	void Update () {
        m_LeftTrigger.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.LeftTrigger];
        m_RightTrigger.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.RightTrigger];

        m_LeftStickX.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.LStickX];
        m_LeftStickY.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.LStickY];

        m_RightStickX.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.RStickX];
        m_RightStickY.value = m_Test.m_Controller.axesValues[(int)ControllerAxes.RStickY];

        int hat = m_Test.m_Controller.hatSwitch;
        m_PovText.text = test.getNameFromHat(hat);

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

    }
}
